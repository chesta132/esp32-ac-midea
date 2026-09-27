#pragma once

#include <Arduino.h>
#include <FormatLog.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <arduino-timer.h>
#include <ir_Midea.h>

#include "helper.hpp"

class AcSharedService {
 public:
  AcSharedService(IRMideaAC ac_) : ac(ac_) {}

  void begin(void (*blynkVirtualWrite)(uint8_t, int)) {
    blynkVirtualWrite_ = blynkVirtualWrite;
  }

  void setSleep(bool on, uint8_t virtualPin) {
    bool prev = ac.getSleep();
    if (prev == on) return;
    ac.setSleep(on);

    if (on) {
      context.self = this;
      context.pin = virtualPin;

      sleepTask_ = timer.in(8_h, sleepTimeout, &context);
    } else if (sleepTask_ != nullptr) {
      timer.cancel(sleepTask_);
    }
  }

  void loop() { timer.tick(); }

 private:
  IRMideaAC ac;
  Timer<> timer = timer_create_default();
  void* sleepTask_;

  struct TimerContext {
    AcSharedService* self;
    uint8_t pin;
  } context;

  static bool sleepTimeout(void* opaque) {
    auto* ctx = static_cast<TimerContext*>(opaque);
    if (!ctx || !ctx->self) return false;

    ctx->self->ac.setSleep(false);
    if (ctx->self->blynkVirtualWrite_) {
      ctx->self->blynkVirtualWrite_(ctx->pin, 0);
    }

    LOG_INFO("AC sleep turned off after 8h");
    return false;
  }

  void (*blynkVirtualWrite_)(uint8_t, int) = nullptr;
};
