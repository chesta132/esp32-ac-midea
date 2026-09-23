#ifndef CONF_HPP
#define CONF_HPP

#include <stdint.h>

struct AcState {
  bool power;
  uint8_t temp;
  uint8_t mode;
  uint8_t fanSpeed;
  bool swing;
  bool sleep;
  bool clean;
  bool ledDisplay;
  double timerOn;
  double timerOff;
};

extern AcState acControllerState;
extern AcState acReceiverState;

// true: acControllerState is the state that controlling the AC
// false: acReceiverState is the state that controlling the AC
extern bool acEspOnControl;

#endif