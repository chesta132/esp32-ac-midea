#pragma once

#include <Arduino.h>

String modeToString(uint8_t);
String fanSpeedToString(uint8_t);

enum AcPin : uint8_t {
  // Controller Pins (V0 - V9)
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

  // Remote Pins (V10 - V19)
  PIN_REMOTE_POWER,
  PIN_REMOTE_TEMP,
  PIN_REMOTE_MODE,
  PIN_REMOTE_FAN,
  PIN_REMOTE_SWING,
  PIN_REMOTE_SLEEP,
  PIN_REMOTE_CLEAN,
  PIN_REMOTE_LED,
  PIN_REMOTE_TIMER_ON,
  PIN_REMOTE_TIMER_OFF,

  // System Control Pin (V20)
  PIN_ESP_ON_CONTROL,

  // Total count indicator for SyncGuard
  PIN_COUNT
};

// DO NOT USE THIS ENUM ON BLYNK_WRITE
// use V0-Vn instead
enum VirtualPin : uint8_t {
  // ESP controller state

  VPPowerSwitch = 0,  // V0
  VPTemperature,      // V1
  VPMode,             // V2
  VPFanSpeed,         // V3
  VPSwing,            // V4
  VPSleep,            // V5
  VPClean,            // V6
  VPLedDisplay,       // V7
  VPTimerOn,          // V8
  VPTimerOff,         // V9

  // AC factory's remote state

  VPFPowerSwitch,  // V10
  VPFTemperature,  // V11
  VPFMode,         // V12
  VPFFanSpeed,     // V13
  VPFSwing,        // V14
  VPFSleep,        // V15
  VPFClean,        // V16
  VPFLedDisplay,   // V17
  VPFTimerOn,      // V18
  VPFTimerOff,     // V19

  VPControlOnESP  // V20
};