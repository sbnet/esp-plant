#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <math.h>

#include "ConfigStore.h"
#include "DisplayManager.h"
#include "EmotionEngine.h"
#include "RestService.h"
#include "WifiSecrets.h"

DisplayManager gDisplay;
EmotionEngine gEmotion;
ConfigStore gConfigStore;
RestService gRest;

EmotionThresholds gThresholds = defaultEmotionThresholds();

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(kWifiSsid, kWifiPassword);

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 12000) {
    delay(200);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi not connected, REST endpoints unavailable");
  }
}

PlantReadings demoReadings(uint32_t nowMillis) {
  const float t = nowMillis * 0.001f;
  const float moisture = 50.0f + sinf(t * 0.20f) * 30.0f;
  const float light = 45.0f + cosf(t * 0.13f) * 35.0f;

  PlantReadings readings{};
  readings.moisturePct = constrain(moisture, 0.0f, 100.0f);
  readings.lightPct = constrain(light, 0.0f, 100.0f);
  return readings;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  gDisplay.begin();

  gConfigStore.begin();
  gThresholds = gConfigStore.load();
  gEmotion.setThresholds(gThresholds);

  connectWifi();
  gRest.begin(80);
  gRest.setThresholds(gThresholds);
}

void loop() {
  const uint32_t now = millis();
  const PlantReadings readings = demoReadings(now);
  const VisualState state = gEmotion.stateFromReadings(readings);

  gRest.updateSnapshot(readings, state);
  gRest.handleClient();

  if (gRest.hasPendingThresholdUpdate()) {
    const EmotionThresholds incoming = gRest.pendingThresholds();
    gThresholds = incoming;
    gEmotion.setThresholds(gThresholds);
    gRest.setThresholds(gThresholds);
    gConfigStore.save(gThresholds);
    gRest.clearPendingThresholdUpdate();
  }

  gDisplay.renderFace(state, now);
  delay(20);
}
