#pragma once

#include <Arduino.h>
#include "SSD1306Wire.h"
#include "PlantTypes.h"

class DisplayManager {
public:
  DisplayManager(uint8_t address = 0x3C, uint8_t sdaPin = D2, uint8_t sclPin = D1);

  void begin();
  void renderFace(VisualState state, uint32_t tMillis);

private:
  enum class EyeStyle : uint8_t {
    GlossyOpen = 0,
    SoftHalf,
    SoftClosed
  };

  SSD1306Wire display_;

  static EyeStyle blendBlinkStyle(uint32_t cycle, uint32_t start, uint32_t duration);

  void drawGlossyOpenEye(int cx, int cy, int pdx, int pdy);
  void drawSoftHalfEye(int cx, int cy);
  void drawSoftClosedEye(int cx, int cy);
  void drawTinySmile(int cx, int cy);
  void drawTinyFlat(int cx, int cy);
  void drawTinyOpen(int cx, int cy);
  void drawEyeStyle(EyeStyle style, int cx, int cy, int gazeX, int gazeY);
};

