#include <Arduino.h>
#include <BlynkSimpleEsp32.h>
#include <FormatLog.h>

#include <functional>
#include <type_traits>

#include "config.hpp"
#include "enum.hpp"
#include "service/ac_controller.hpp"
#include "service/ac_receiver.hpp"
#include "state.hpp"
#include "sync_guard.hpp"

SyncGuard syncGuard;
const unsigned long SYNC_TIMEOUT_MS = 5000;

void setup() {
  Serial.begin(115200);
  LOG_BEGIN(9600);

  acControl.begin();
  acReceive.begin(
      [](uint8_t pin, int value) { Blynk.virtualWrite(pin, value); });
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
}

void loop() {
  Blynk.run();
  syncGuard.checkTimeout(SYNC_TIMEOUT_MS);
  acReceive.loop();
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
                    bool isLowPriority = true,
                    std::function<String(T)> toStr = nullptr) {
  applyFunc(val);
  stateVar = val;
  String val_in_log = toStr ? toStr(val) : String(val);
  Serial1.println(3);

  if (std::is_same<T, bool>::value) {
    LOG_INFO("AC {} state changed to: {}", name, val ? "ON" : "OFF");
  } else {
    LOG_INFO("AC {} changed to: {}", name, val_in_log.c_str());
  }

  if (isLowPriority) {
    acControl.sendLowPriority(syncGuard);
  } else {
    acControl.send(syncGuard);
  }

  Serial1.println(4);
  syncGuard.tick(pin);
  Serial1.println(5);
}

// Blynk pin handlers
BLYNK_WRITE(V0) {  // Power
  Serial1.println(1);
  handleAcUpdate<bool>(
      PIN_POWER, param.asInt() == 1, acControllerState.power,
      [](bool v) {
        Serial1.println(2);
        if (v)
          acControl.ac.on();
        else
          acControl.ac.off();
      },
      "power", false);
}

BLYNK_WRITE(V1) {  // Power
  handleAcUpdate<uint8_t>(
      PIN_TEMP, param.asInt(), acControllerState.temp,
      [](uint8_t v) { acControl.ac.setTemp(v, true); },
      "temperature (celsius)");
}

BLYNK_WRITE(V2) {  // Mode
  handleAcUpdate<uint8_t>(
      PIN_MODE, param.asInt(), acControllerState.mode,
      [](uint8_t v) { acControl.ac.setMode(v); }, "mode", true, modeToString);
}

BLYNK_WRITE(V3) {  // Fan Speed
  handleAcUpdate<uint8_t>(
      PIN_FAN, param.asInt(), acControllerState.fanSpeed,
      [](uint8_t v) { acControl.ac.setFan(v); }, "fan speed", true,
      fanSpeedToString);
}

BLYNK_WRITE(V4) {  // Swing
  handleAcUpdate<bool>(
      PIN_SWING, param.asInt() == 1, acControllerState.swing,
      [](bool v) { acControl.ac.setSwingVToggle(v); }, "swing");
}

BLYNK_WRITE(V5) {  // Sleep
  handleAcUpdate<bool>(
      PIN_SLEEP, param.asInt() == 1, acControllerState.sleep,
      [](bool v) { acControl.ac.setSleep(v); }, "sleep");
}

BLYNK_WRITE(V6) {  // Clean
  handleAcUpdate<bool>(
      PIN_CLEAN, param.asInt() == 1, acControllerState.clean,
      [](bool v) { acControl.ac.setCleanToggle(v); }, "clean");
}

BLYNK_WRITE(V7) {  // LED Display
  handleAcUpdate<bool>(
      PIN_LED, param.asInt() == 1, acControllerState.ledDisplay,
      [](bool v) { acControl.ac.setLightToggle(v); }, "LED display");
}

BLYNK_WRITE(V8) {  // Timer on (param: hours, ac: minutes)
  uint16_t dummyState;
  uint16_t minutes = static_cast<uint16_t>(round(param.asDouble() * 60.0));
  handleAcUpdate<uint16_t>(
      PIN_TIMER_ON, minutes, dummyState,
      [](uint16_t v) { acControl.ac.setOnTimer(v); }, "timer on (minutes)");
}

BLYNK_WRITE(V9) {  // Timer off (param: hours, ac: minutes)
  uint16_t dummyState;
  uint16_t minutes = static_cast<uint16_t>(round(param.asDouble() * 60.0));
  handleAcUpdate<uint16_t>(
      PIN_TIMER_OFF, minutes, dummyState,
      [](uint16_t v) { acControl.ac.setOffTimer(v); }, "timer off (minutes)");
}