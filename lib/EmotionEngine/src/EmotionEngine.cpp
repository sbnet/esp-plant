#include "EmotionEngine.h"

EmotionEngine::EmotionEngine() : thresholds_(defaultEmotionThresholds()) {}

void EmotionEngine::setThresholds(const EmotionThresholds& thresholds) {
  if (isValidEmotionThresholds(thresholds)) {
    thresholds_ = thresholds;
  }
}

const EmotionThresholds& EmotionEngine::thresholds() const {
  return thresholds_;
}

VisualState EmotionEngine::stateFromReadings(const PlantReadings& readings) const {
  if (readings.moisturePct < thresholds_.dryMoistureMax) {
    return VisualState::Dry;
  }

  if (readings.lightPct < thresholds_.lowLightMax) {
    return VisualState::LowLight;
  }

  if (readings.moisturePct >= thresholds_.goodMoistureMin + 15.0f) {
    return VisualState::Excellent;
  }

  if (readings.moisturePct >= thresholds_.goodMoistureMin) {
    return VisualState::Good;
  }

  return VisualState::Okay;
}

VisualState EmotionEngine::stateFromDemo(uint32_t tMillis) const {
  const uint32_t cycle = tMillis % 15000;

  if (cycle < 3000) {
    return VisualState::Dry;
  }
  if (cycle < 6000) {
    return VisualState::LowLight;
  }
  if (cycle < 9000) {
    return VisualState::Okay;
  }
  if (cycle < 12000) {
    return VisualState::Good;
  }
  return VisualState::Excellent;
}

