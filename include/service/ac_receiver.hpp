#pragma once

#include <Arduino.h>
#include <FormatLog.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <ir_Midea.h>

#include "enum.hpp"
#include "state.hpp"

#define SYNC_IF_CHANGED(field, vpin, ...) \
  if (field != acControllerState.field) blynkVirtualWrite_(vpin, __VA_ARGS__)

extern decode_results results;

class AcReceiverService {
 public:
  AcReceiverService(uint8_t irPin) : ac(irPin), irrecv(irPin) {}

  void begin(void (*blynkVirtualWrite)(uint8_t, int)) {
    blynkVirtualWrite_ = blynkVirtualWrite;
    irrecv.enableIRIn();
    LOG_INFO("Midea AC IR receiver ready.");
  }

  void loop() {
    if (irrecv.decode(&results)) {
      if (results.decode_type != MIDEA) {
        irrecv.resume();
        return;
      }

      ac.setRaw(results.value);

      bool power = ac.getPower();
      uint8_t temp = ac.getTemp();
      uint8_t mode = ac.getMode();
      uint8_t fanSpeed = ac.getFan();
      bool swing = ac.getSwingVToggle();
      bool sleep = ac.getSleep();
      bool clean = ac.getCleanToggle();
      bool ledDisplay = ac.getLightToggle();
      double timerOn = ac.getOnTimer() / 60;
      double timerOff = ac.getOffTimer() / 60;

      if (acControlState.isEspOnControl()) {
        acControlState.set(false);

        SYNC_IF_CHANGED(power, VPFPowerSwitch, power ? 1 : 0);
        SYNC_IF_CHANGED(temp, VPFTemperature, temp);
        SYNC_IF_CHANGED(mode, VPFMode, mode);
        SYNC_IF_CHANGED(fanSpeed, VPFFanSpeed, fanSpeed);
        SYNC_IF_CHANGED(swing, VPFSwing, swing ? 1 : 0);
        SYNC_IF_CHANGED(sleep, VPFSleep, sleep ? 1 : 0);
        SYNC_IF_CHANGED(clean, VPFClean, clean ? 1 : 0);
        SYNC_IF_CHANGED(ledDisplay, VPFLedDisplay, ledDisplay ? 1 : 0);
        SYNC_IF_CHANGED(timerOn, VPFTimerOn, timerOn);
        SYNC_IF_CHANGED(timerOff, VPFTimerOff, timerOff);

        LOG_INFO("AC factory's remote is taking control.");
      }

      acReceiverState.power = power;
      acReceiverState.temp = temp;
      acReceiverState.mode = mode;
      acReceiverState.fanSpeed = fanSpeed;
      acReceiverState.swing = swing;
      acReceiverState.sleep = sleep;
      acReceiverState.clean = clean;
      acReceiverState.ledDisplay = ledDisplay;

      irrecv.resume();
    }
  }

  IRMideaAC ac;

 private:
  IRrecv irrecv;
  void (*blynkVirtualWrite_)(uint8_t, int);
};

extern AcReceiverService acReceive;