#include "RestService.h"

RestService::RestService()
    : server_(80),
      lastReadings_{50.0f, 50.0f},
      lastState_(VisualState::Okay),
      activeThresholds_(defaultEmotionThresholds()),
      hasPendingThresholdUpdate_(false),
      pendingThresholds_(defaultEmotionThresholds()) {}

void RestService::begin(uint16_t port) {
  (void)port;
  configureRoutes();
  server_.begin();
}

void RestService::handleClient() {
  server_.handleClient();
}

void RestService::updateSnapshot(const PlantReadings& readings, VisualState state) {
  lastReadings_ = readings;
  lastState_ = state;
}

void RestService::setThresholds(EmotionThresholds thresholds) {
  activeThresholds_ = thresholds;
}

const EmotionThresholds& RestService::pendingThresholds() const {
  return pendingThresholds_;
}

bool RestService::hasPendingThresholdUpdate() const {
  return hasPendingThresholdUpdate_;
}

void RestService::clearPendingThresholdUpdate() {
  hasPendingThresholdUpdate_ = false;
}

void RestService::configureRoutes() {
  server_.on("/status", HTTP_GET, [this]() { handleGetStatus(); });
  server_.on("/config", HTTP_POST, [this]() { handlePostConfig(); });
  server_.onNotFound([this]() { handleNotFound(); });
}

void RestService::handleGetStatus() {
  String payload = "{";
  payload += "\"state\":\"" + String(visualStateToString(lastState_)) + "\",";
  payload += "\"readings\":{";
  payload += "\"moisturePct\":" + String(lastReadings_.moisturePct, 2) + ",";
  payload += "\"lightPct\":" + String(lastReadings_.lightPct, 2);
  payload += "},";
  payload += "\"thresholds\":{";
  payload += "\"dryMoistureMax\":" + String(activeThresholds_.dryMoistureMax, 2) + ",";
  payload += "\"goodMoistureMin\":" + String(activeThresholds_.goodMoistureMin, 2) + ",";
  payload += "\"lowLightMax\":" + String(activeThresholds_.lowLightMax, 2);
  payload += "}";
  payload += "}";

  server_.send(200, "application/json", payload);
}

void RestService::handlePostConfig() {
  const String body = server_.arg("plain");

  float dryMoistureMax = 0.0f;
  float goodMoistureMin = 0.0f;
  float lowLightMax = 0.0f;

  const bool ok = parseThresholdField(body, "dryMoistureMax", dryMoistureMax) &&
                  parseThresholdField(body, "goodMoistureMin", goodMoistureMin) &&
                  parseThresholdField(body, "lowLightMax", lowLightMax);

  if (!ok) {
    server_.send(400, "application/json", "{\"error\":\"Invalid payload\"}");
    return;
  }

  EmotionThresholds incoming{dryMoistureMax, goodMoistureMin, lowLightMax};
  if (!isValidEmotionThresholds(incoming)) {
    server_.send(400, "application/json", "{\"error\":\"Invalid threshold values\"}");
    return;
  }

  pendingThresholds_ = incoming;
  hasPendingThresholdUpdate_ = true;

  String payload = "{";
  payload += "\"ok\":true,";
  payload += "\"thresholds\":{";
  payload += "\"dryMoistureMax\":" + String(incoming.dryMoistureMax, 2) + ",";
  payload += "\"goodMoistureMin\":" + String(incoming.goodMoistureMin, 2) + ",";
  payload += "\"lowLightMax\":" + String(incoming.lowLightMax, 2);
  payload += "}";
  payload += "}";

  server_.send(200, "application/json", payload);
}

void RestService::handleNotFound() {
  server_.send(404, "application/json", "{\"error\":\"Not found\"}");
}

bool RestService::parseThresholdField(const String& body, const char* key, float& value) {
  const String pattern = String("\"") + key + "\":";
  const int start = body.indexOf(pattern);
  const int bodyLength = static_cast<int>(body.length());
  if (start < 0) {
    return false;
  }

  int valueStart = start + pattern.length();
  while (valueStart < bodyLength && body[valueStart] == ' ') {
    ++valueStart;
  }

  int valueEnd = valueStart;

  while (valueEnd < bodyLength) {
    const char c = body[valueEnd];
    const bool isNumericChar = (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '+';
    if (!isNumericChar) {
      break;
    }
    ++valueEnd;
  }

  if (valueEnd == valueStart) {
    return false;
  }

  const String number = body.substring(valueStart, valueEnd);
  value = number.toFloat();
  return true;
}

