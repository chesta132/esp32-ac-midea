#pragma once

#include <WiFi.h>

void initializeWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
}

void connectWifi() { WiFi.begin(WIFI_SSID, WIFI_PASS); }