#pragma once

#include <Arduino.h>
#include "PlantTypes.h"

class EmotionEngine {
public:
  EmotionEngine();

  void setThresholds(const EmotionThresholds& thresholds);
  const EmotionThresholds& thresholds() const;

  VisualState stateFromReadings(const PlantReadings& readings) const;

  // Temporary demo: automatic visual state cycle.
  // Replace later with sensor-driven logic.
  VisualState stateFromDemo(uint32_t tMillis) const;

private:
  EmotionThresholds thresholds_;
};

