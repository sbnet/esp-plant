#pragma once

#include <Arduino.h>

enum class VisualState : uint8_t {
  Dry = 0,
  LowLight,
  Okay,
  Good,
  Excellent
};

inline const char* visualStateToString(VisualState state) {
  switch (state) {
    case VisualState::Dry:
      return "dry";
    case VisualState::LowLight:
      return "low_light";
    case VisualState::Okay:
      return "okay";
    case VisualState::Good:
      return "good";
    case VisualState::Excellent:
      return "excellent";
  }
  return "unknown";
}

struct PlantReadings {
  float moisturePct;
  float lightPct;
};

struct EmotionThresholds {
  float dryMoistureMax;
  float goodMoistureMin;
  float lowLightMax;
};

inline EmotionThresholds defaultEmotionThresholds() {
  return EmotionThresholds{
      30.0f, // dryMoistureMax
      65.0f, // goodMoistureMin
      20.0f  // lowLightMax
  };
}

inline bool isValidEmotionThresholds(const EmotionThresholds& thresholds) {
  if (thresholds.dryMoistureMax < 0.0f || thresholds.dryMoistureMax > 100.0f) return false;
  if (thresholds.goodMoistureMin < 0.0f || thresholds.goodMoistureMin > 100.0f) return false;
  if (thresholds.lowLightMax < 0.0f || thresholds.lowLightMax > 100.0f) return false;
  if (thresholds.dryMoistureMax >= thresholds.goodMoistureMin) return false;
  return true;
}
