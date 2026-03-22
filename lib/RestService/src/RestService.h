#pragma once

#include <Arduino.h>
#include <ESP8266WebServer.h>

#include "EmotionEngine.h"
#include "PlantTypes.h"

class RestService {
public:
  RestService();

  void begin(uint16_t port = 80);
  void handleClient();

  void updateSnapshot(const PlantReadings& readings, VisualState state);
  void setThresholds(EmotionThresholds thresholds);

  const EmotionThresholds& pendingThresholds() const;
  bool hasPendingThresholdUpdate() const;
  void clearPendingThresholdUpdate();

private:
  ESP8266WebServer server_;
  PlantReadings lastReadings_;
  VisualState lastState_;
  EmotionThresholds activeThresholds_;

  bool hasPendingThresholdUpdate_;
  EmotionThresholds pendingThresholds_;

  void configureRoutes();
  void handleGetStatus();
  void handlePostConfig();
  void handleNotFound();

  static bool parseThresholdField(const String& body, const char* key, float& value);
};

