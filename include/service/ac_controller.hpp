#pragma once

#include <Arduino.h>
#include <FormatLog.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Midea.h>

#include "config.hpp"
#include "state.hpp"
#include "sync_guard.hpp"

class AcControllerService {
 public:
  AcControllerService(uint8_t irPin) : ac(irPin) {}

  void begin() {
    ac.begin();
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

  IRMideaAC ac;

 private:
  void beforeSend_() {
    // if (!acControlState.isEspOnControl()) {
    //   acControlState.set(true);
    //   LOG_INFO("AC ESP is taking control.");
    // }
  }
};

extern AcControllerService acControl;