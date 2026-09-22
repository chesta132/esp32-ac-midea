#include <Arduino.h>
#include <BlynkSimpleEsp32.h>
#include <FormatLog.h>

#include <functional>
#include <type_traits>

#include "ac_service.hpp"
#include "config.hpp"
#include "state.hpp"
#include "sync_guard.hpp"

SyncGuard syncGuard;
const unsigned long SYNC_TIMEOUT_MS = 5000;

void setup() {
  Serial.begin(115200);
  LOG_BEGIN(9600);

  acService.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
}

void loop() {
  Blynk.run();
  syncGuard.checkTimeout(SYNC_TIMEOUT_MS);
}

BLYNK_CONNECTED() {
  LOG_INFO("Blynk connected, syncing virtual pins.");
  syncGuard.begin(PIN_COUNT);
  syncGuard.onFinished([](SyncGuard::Callback pc) {
    LOG_INFO("Sync finished.");
    if (pc) pc();
  });
  Blynk.syncAll();
}

// generic BLYNK_WRITE handler
template <typename T>
void handleAcUpdate(AcPin pin, T val, T& stateVar,
                    std::function<void(T)> applyFunc, const char* name,
                    bool isLowPriority = true) {
  applyFunc(val);
  stateVar = val;

  if (std::is_same<T, bool>::value) {
    LOG_INFO("AC {} state changed to: {}", name, val ? "ON" : "OFF");
  } else {
    LOG_INFO("AC {} changed to: {}", name, val);
  }

  if (isLowPriority) {
    acService.sendLowPriority(syncGuard);
  } else {
    acService.send(syncGuard);
  }

  syncGuard.tick(pin);
}

// Blynk pin handlers
BLYNK_WRITE(V0) {  // Power
  handleAcUpdate<bool>(
      PIN_POWER, param.asInt() == 1, acState.power,
      [](bool v) {
        if (v)
          acService.ac.on();
        else
          acService.ac.off();
      },
      "power", false);
}

BLYNK_WRITE(V1) {  // Temperature
  handleAcUpdate<uint8_t>(
      PIN_TEMP, param.asInt(), acState.temp,
      [](uint8_t v) { acService.ac.setTemp(v); }, "temperature");
}

BLYNK_WRITE(V2) {  // Mode
  handleAcUpdate<uint8_t>(
      PIN_MODE, param.asInt(), acState.mode,
      [](uint8_t v) { acService.ac.setMode(v); }, "mode");
}

BLYNK_WRITE(V3) {  // Fan Speed
  handleAcUpdate<uint8_t>(
      PIN_FAN, param.asInt(), acState.fan_speed,
      [](uint8_t v) { acService.ac.setFan(v); }, "fan speed");
}

BLYNK_WRITE(V4) {  // Swing
  handleAcUpdate<bool>(
      PIN_SWING, param.asInt() == 1, acState.swing,
      [](bool v) { acService.ac.setSwingVToggle(v); }, "swing");
}

BLYNK_WRITE(V5) {  // Sleep
  handleAcUpdate<bool>(
      PIN_SLEEP, param.asInt() == 1, acState.sleep,
      [](bool v) { acService.ac.setSleep(v); }, "sleep");
}

BLYNK_WRITE(V6) {  // Clean
  handleAcUpdate<bool>(
      PIN_CLEAN, param.asInt() == 1, acState.clean,
      [](bool v) { acService.ac.setCleanToggle(v); }, "clean");
}

BLYNK_WRITE(V7) {  // LED Display
  handleAcUpdate<bool>(
      PIN_LED, param.asInt() == 1, acState.ledDisplay,
      [](bool v) { acService.ac.setLightToggle(v); }, "LED display");
}