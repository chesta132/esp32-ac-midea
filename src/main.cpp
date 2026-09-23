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

  auto blynkVirtualWrite = [](uint8_t pin, int value) {
    Blynk.virtualWrite(pin, value);
  };

  acControlState.begin(VPControlOnESP, blynkVirtualWrite);
  acControl.begin();
  acReceive.begin(blynkVirtualWrite);
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
}

void loop() {
  Blynk.run();
  syncGuard.checkTimeout(SYNC_TIMEOUT_MS);
  acReceive.loop();
}

BLYNK_CONNECTED() {
  LOG_INFO(
      "Blynk connected, syncing virtual pins. Can not send IR while syncing.");
  syncGuard.begin(PIN_COUNT);
  syncGuard.onFinished([](SyncGuard::Callback pc) {
    LOG_INFO("Sync finished.\n");
    if (acControlState.isEspOnControl()) acControl.send(syncGuard);
    if (pc) pc();
  });
  Blynk.syncAll();
}

// generic BLYNK_WRITE handler
template <typename T>
void handleAcUpdate(AcPin pin, T val, T& stateVar,
                    std::function<void(T)> applyFunc, const char* name,
                    bool isLowPriority = true,
                    std::function<String(T)> toStr = nullptr,
                    bool send = true) {
  applyFunc(val);
  stateVar = val;
  String val_in_log = toStr ? toStr(val) : String(val);

  if (std::is_same<T, bool>::value) {
    LOG_INFO("AC {} state changed to: {}", name, val ? "ON" : "OFF");
  } else {
    LOG_INFO("AC {} changed to: {}", name, val_in_log.c_str());
  }

  if (send && !syncGuard.isSyncing()) {
    if (isLowPriority) {
      acControl.sendLowPriority(syncGuard);
    } else {
      acControl.send(syncGuard);
    }
  }

  syncGuard.tick(pin);
}

// Blynk pin handlers
BLYNK_WRITE(V0) {  // Power
  handleAcUpdate<bool>(
      PIN_POWER, param.asInt() == 1, acControllerState.power,
      [](bool v) {
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

// Remote (sync only)
BLYNK_WRITE(V10) {  // Remote Power
  handleAcUpdate<bool>(
      PIN_REMOTE_POWER, param.asInt() == 1, acReceiverState.power,
      [](bool v) {
        if (v)
          acReceive.ac.on();
        else
          acReceive.ac.off();
      },
      "remote power", false, nullptr, false);
}

BLYNK_WRITE(V11) {  // Remote Temp
  handleAcUpdate<uint8_t>(
      PIN_REMOTE_TEMP, param.asInt(), acReceiverState.temp,
      [](uint8_t v) { acReceive.ac.setTemp(v, true); },
      "remote temperature (celsius)", true, nullptr, false);
}

BLYNK_WRITE(V12) {  // Remote Mode
  handleAcUpdate<uint8_t>(
      PIN_REMOTE_MODE, param.asInt(), acReceiverState.mode,
      [](uint8_t v) { acReceive.ac.setMode(v); }, "remote mode", true,
      modeToString, false);
}

BLYNK_WRITE(V13) {  // Remote Fan Speed
  handleAcUpdate<uint8_t>(
      PIN_REMOTE_FAN, param.asInt(), acReceiverState.fanSpeed,
      [](uint8_t v) { acReceive.ac.setFan(v); }, "remote fan speed", true,
      fanSpeedToString, false);
}

BLYNK_WRITE(V14) {  // Remote Swing
  handleAcUpdate<bool>(
      PIN_REMOTE_SWING, param.asInt() == 1, acReceiverState.swing,
      [](bool v) { acReceive.ac.setSwingVToggle(v); }, "remote swing", true,
      nullptr, false);
}

BLYNK_WRITE(V15) {  // Remote Sleep
  handleAcUpdate<bool>(
      PIN_REMOTE_SLEEP, param.asInt() == 1, acReceiverState.sleep,
      [](bool v) { acReceive.ac.setSleep(v); }, "remote sleep", true, nullptr,
      false);
}

BLYNK_WRITE(V16) {  // Remote Clean
  handleAcUpdate<bool>(
      PIN_REMOTE_CLEAN, param.asInt() == 1, acReceiverState.clean,
      [](bool v) { acReceive.ac.setCleanToggle(v); }, "remote clean", true,
      nullptr, false);
}

BLYNK_WRITE(V17) {  // Remote LED Display
  handleAcUpdate<bool>(
      PIN_REMOTE_LED, param.asInt() == 1, acReceiverState.ledDisplay,
      [](bool v) { acReceive.ac.setLightToggle(v); }, "remote LED display",
      true, nullptr, false);
}

BLYNK_WRITE(V18) {  // Remote Timer on (param: hours, ac: minutes)
  uint16_t dummyState;
  uint16_t minutes = static_cast<uint16_t>(round(param.asDouble() * 60.0));
  handleAcUpdate<uint16_t>(
      PIN_REMOTE_TIMER_ON, minutes, dummyState,
      [](uint16_t v) { acReceive.ac.setOnTimer(v); },
      "remote timer on (minutes)", true, nullptr, false);
}

BLYNK_WRITE(V19) {  // Remote Timer off (param: hours, ac: minutes)
  uint16_t dummyState;
  uint16_t minutes = static_cast<uint16_t>(round(param.asDouble() * 60.0));
  handleAcUpdate<uint16_t>(
      PIN_REMOTE_TIMER_OFF, minutes, dummyState,
      [](uint16_t v) { acReceive.ac.setOffTimer(v); },
      "remote timer off (minutes)", true, nullptr, false);
}

// Middle

BLYNK_WRITE(V20) {  // ESP On Control
  bool dummyState;
  bool value = param.asInt() == 1;
  handleAcUpdate<bool>(
      PIN_ESP_ON_CONTROL, value, dummyState,
      [](bool v) { acControlState.set(v, true); }, "ESP on control", false,
      // only send on esp on control
      nullptr, value);
}