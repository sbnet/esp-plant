#pragma once

#include <Arduino.h>

enum class VisualState : uint8_t {
  Dry = 0,
  LowLight,
  Okay,
  Good,
  Excellent
};

