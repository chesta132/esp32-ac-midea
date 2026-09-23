#pragma once

#include <Arduino.h>

String modeToString(uint8_t);
String fanSpeedToString(uint8_t);

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