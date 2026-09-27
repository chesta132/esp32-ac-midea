#pragma once

#include <Arduino.h>
#include <FormatLog.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <arduino-timer.h>
#include <ir_Midea.h>

#include "config.hpp"
#include "enum.hpp"
#include "helper.hpp"
#include "service/ac_shared.hpp"
#include "state.hpp"
#include "sync_guard.hpp"

class AcControllerService {
 public:
  AcControllerService(uint8_t irPin) : ac(irPin), shared(ac) {}

  void begin(void (*blynkVirtualWrite)(uint8_t, int)) {
    ac.begin();
    shared.begin(blynkVirtualWrite);
    ac.setUseCelsius(true);
    LOG_INFO("Midea AC IR controller ready.");
  }

  void send(SyncGuard& syncGuard) {
    if (syncGuard.isSyncing()) {
      LOG_INFO("Syncing, skipping sending IR command.");
      return;
    }

    if (!acControlState.isEspOnControl()) {
      LOG_INFO("ESP is not on control, skipping sending IR command.");
      return;
    }

    beforeSend_();
    ac.send();
  }

  void sendLowPriority(SyncGuard& syncGuard) {
    if (syncGuard.isSyncing()) {
      LOG_INFO("Syncing, skipping sending IR command.");
      return;
    }

    if (!acControlState.isEspOnControl()) {
      LOG_INFO("ESP is not on control, skipping sending IR command.");
      return;
    }

    if (acControllerState.power) {
      beforeSend_();
      ac.send();
    } else {
      LOG_INFO("Change's not applied because AC is OFF.");
    }
  }

  void setSleep(bool on) { shared.setSleep(on, VirtualPin::VPSleep); }

  void loop() { shared.loop(); }

  IRMideaAC ac;

 private:
  void beforeSend_() {
    if (!acControlState.isEspOnControl()) {
      acControlState.set(true);
      LOG_INFO("AC ESP is taking control.");
    }
  }

  void (*blynkVirtualWrite_)(uint8_t, int);
  AcSharedService shared;
};

extern AcControllerService acControl;