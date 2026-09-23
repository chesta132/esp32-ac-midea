#ifndef CONF_HPP
#define CONF_HPP

#include <stdint.h>
#include <FormatLog.h>

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

class ControlModeState {
 public:
  void begin(uint8_t pin, void (*blynkVirtualWrite)(uint8_t, int)) {
    pin_ = pin;
    blynkVirtualWrite_ = blynkVirtualWrite;
  }

  bool isEspOnControl() const { return value_; }

  // Single entry point to change state.
  void set(bool newValue, bool fromBlynk = false) {
    if (value_ == newValue) return;
    value_ = newValue;
    if (!fromBlynk) {
      blynkVirtualWrite_(pin_, value_ ? 1 : 0);
    }
    LOG_INFO("Control mode changed to: {}", value_ ? "ESP" : "Remote");
  }

 private:
  bool value_ = false;
  uint8_t pin_ = 0;
  void (*blynkVirtualWrite_)(uint8_t, int);
};

extern ControlModeState acControlState;

#endif