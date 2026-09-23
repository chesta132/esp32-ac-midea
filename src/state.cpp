#include "state.hpp"

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <ir_Midea.h>

AcState acState = {.power = false,
                   .temp = 25,
                   .mode = kMideaACAuto,
                   .fan_speed = kMideaACFanAuto,
                   .swing = false,
                   .sleep = false,
                   .clean = false,
                   .ledDisplay = true};
