#include "enum.hpp"

String modeToString(uint8_t mode) {
  switch (mode) {
    case 0:
      return "COOL";
    case 1:
      return "DRY";
    case 2:
      return "AUTO";
    case 3:
      return "HEAT";
    case 4:
      return "FAN";
    default:
      return "AUTO";
  }
}

String fanSpeedToString(uint8_t fanSpeed) {
  switch (fanSpeed) {
    case 0:
      return "AUTO";
    case 1:
      return "LOW";
    case 2:
      return "MEDIUM";
    case 3:
      return "HIGH";
    default:
      return "AUTO";
  }
}
