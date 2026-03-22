#include "EmotionEngine.h"

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

