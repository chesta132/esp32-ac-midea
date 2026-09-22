#ifndef CONF_HPP
#define CONF_HPP

#include <stdint.h>

struct AcState {
  bool power;         // v0
  uint8_t temp;       // v1
  uint8_t mode;       // v2
  uint8_t fan_speed;  // v3
  bool swing;         // v4
  bool sleep;         // v5
  bool clean;         // v6
  bool ledDisplay;   // v7
};

AcState acState;

#endif