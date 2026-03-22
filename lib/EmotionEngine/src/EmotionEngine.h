#pragma once

#include <Arduino.h>
#include "PlantTypes.h"

class EmotionEngine {
public:
  EmotionEngine() = default;

  // Temporary demo: automatic visual state cycle.
  // Replace later with sensor-driven logic.
  VisualState stateFromDemo(uint32_t tMillis) const;
};

