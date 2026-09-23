#include "state.hpp"

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <ir_Midea.h>

AcState acControllerState = {.power = false,
                             .temp = 25,
                             .mode = kMideaACAuto,
                             .fanSpeed = kMideaACFanAuto,
                             .swing = false,
                             .sleep = false,
                             .clean = false,
                             .ledDisplay = true,
                             .timerOn = 0.0,
                             .timerOff = 0.0};

AcState acReceiverState = {.power = false,
                           .temp = 25,
                           .mode = kMideaACAuto,
                           .fanSpeed = kMideaACFanAuto,
                           .swing = false,
                           .sleep = false,
                           .clean = false,
                           .ledDisplay = true,
                           .timerOn = 0.0,
                           .timerOff = 0.0};

bool acEspOnControl = false;