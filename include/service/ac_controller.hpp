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
    ac.send();
  }

  void sendLowPriority(SyncGuard& syncGuard) {
    if (syncGuard.isSyncing()) {
      LOG_INFO("Syncing, skipping sending IR command.");
      return;
    }

    if (acState.power) {
      ac.send();
    } else {
      LOG_INFO("Change's not applied because AC is OFF.");
    }
  }

  IRMideaAC ac;
};

extern AcControllerService acControl;