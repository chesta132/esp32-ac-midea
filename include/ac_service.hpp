#pragma once

#include <Arduino.h>
#include <FormatLog.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Midea.h>

#include "config.hpp"
#include "state.hpp"
#include "sync_guard.hpp"

enum AcPin : uint8_t {
  PIN_POWER = 0,
  PIN_TEMP,
  PIN_MODE,
  PIN_FAN,
  PIN_SWING,
  PIN_SLEEP,
  PIN_CLEAN,
  PIN_LED,
  PIN_TIMER_ON,
  PIN_TIMER_OFF,
  PIN_COUNT
};

class AcService {
 public:
  AcService(uint8_t irPin) : ac(irPin) {}

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

extern AcService acService;