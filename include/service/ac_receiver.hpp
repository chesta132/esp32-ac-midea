#pragma once

#include <Arduino.h>
#include <FormatLog.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <ir_Midea.h>

#include "enum.hpp"
#include "service/ac_shared.hpp"
#include "state.hpp"

#define SYNC_IF_CHANGED(field, vpin, ...)                                      \
  if (field != acControllerState.field) blynkVirtualWrite_(vpin, __VA_ARGS__); \
  acReceiverState.field = __VA_ARGS__

extern decode_results results;

class AcReceiverService {
 public:
  AcReceiverService(uint8_t irPin)
      : ac(irPin),
        irrecv(irPin, BUFFER_SIZE_IR_RECEIVER, TIMEOUT_IR_RECEIVER, true),
        shared(ac) {}

  void begin(void (*blynkVirtualWrite)(uint8_t, int)) {
    shared.begin(blynkVirtualWrite);
    blynkVirtualWrite_ = blynkVirtualWrite;
    irrecv.enableIRIn();
    ac.setUseCelsius(true);
    LOG_INFO("Midea AC IR receiver ready.");
  }

  void loop() {
    if (irrecv.decode(&results)) {
      if (results.decode_type == MIDEA || results.decode_type == MIDEA24) {
        ac.setRaw(results.value);

        bool power = ac.getPower();
        uint8_t temp = ac.getTemp(true);
        uint8_t mode = ac.getMode();
        uint8_t fanSpeed = ac.getFan();
        bool swing = ac.getSwingVToggle();
        bool sleep = ac.getSleep();
        bool clean = ac.getCleanToggle();
        bool ledDisplay = ac.getLightToggle();
        double timerOn = ac.getOnTimer() / 60.0;
        double timerOff = ac.getOffTimer() / 60.0;

        if (acControlState.isEspOnControl()) {
          acControlState.set(false);
          LOG_INFO("AC factory's remote is taking control.");
        }

        SYNC_IF_CHANGED(power, VPFPowerSwitch, power ? 1 : 0);
        SYNC_IF_CHANGED(temp, VPFTemperature, temp);
        SYNC_IF_CHANGED(mode, VPFMode, mode);
        SYNC_IF_CHANGED(fanSpeed, VPFFanSpeed, fanSpeed);
        SYNC_IF_CHANGED(swing, VPFSwing, swing ? 1 : 0);
        SYNC_IF_CHANGED(sleep, VPFSleep, sleep ? 1 : 0);
        shared.setSleep(sleep, VPFSleep);
        SYNC_IF_CHANGED(clean, VPFClean, clean ? 1 : 0);
        SYNC_IF_CHANGED(ledDisplay, VPFLedDisplay, ledDisplay ? 1 : 0);
        SYNC_IF_CHANGED(timerOn, VPFTimerOn, timerOn);
        SYNC_IF_CHANGED(timerOff, VPFTimerOff, timerOff);
      }

      irrecv.resume();
    }
  }

  IRMideaAC ac;

 private:
  IRrecv irrecv;
  void (*blynkVirtualWrite_)(uint8_t, int);
  AcSharedService shared;
};

extern AcReceiverService acReceive;