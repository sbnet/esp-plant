#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include <math.h>

// OLED SSD1306 64x48
// Adresse confirmée : 0x3C
// SDA = D2, SCL = D1
SSD1306Wire display(0x3C, D2, D1, GEOMETRY_64_48);

enum VisualState : uint8_t {
  STATE_DRY = 0,
  STATE_LOW_LIGHT,
  STATE_OKAY,
  STATE_GOOD,
  STATE_EXCELLENT
};

enum EyeStyle : uint8_t {
  EYE_GLOSSY_OPEN = 0,
  EYE_SOFT_HALF,
  EYE_SOFT_CLOSED
};

static inline void hLine(int x, int y, int len) {
  display.drawLine(x, y, x + len - 1, y);
}

void drawOpenEye(int cx, int cy, int pdx, int pdy, int r = 5) {
  display.drawCircle(cx, cy, r);
  display.fillCircle(cx + pdx, cy + pdy, 2);
  display.setPixel(cx + pdx + 1, cy + pdy - 1);
}

void drawGlossyOpenEye(int cx, int cy, int pdx, int pdy) {
  // Oeil simple + reflet pour un effet glossy (écran monochrome)
  display.drawCircle(cx, cy, 5);
  display.fillCircle(cx + pdx, cy + pdy, 2);

  // Reflets
  display.setPixel(cx - 2, cy - 2);
  display.setPixel(cx - 1, cy - 3);
  display.setPixel(cx + 2, cy - 2);
}

void drawSoftHalfEye(int cx, int cy) {
  display.drawLine(cx - 5, cy, cx + 5, cy);
  display.drawLine(cx - 4, cy + 1, cx + 4, cy + 1);
}

void drawSoftClosedEye(int cx, int cy) {
  display.drawLine(cx - 5, cy + 1, cx - 1, cy);
  display.drawLine(cx - 1, cy, cx + 1, cy);
  display.drawLine(cx + 1, cy, cx + 5, cy + 1);
}

void drawTinySmile(int cx, int cy) {
  display.setPixel(cx - 2, cy);
  display.setPixel(cx - 1, cy + 1);
  display.setPixel(cx, cy + 1);
  display.setPixel(cx + 1, cy + 1);
  display.setPixel(cx + 2, cy);
}

void drawTinyFlat(int cx, int cy) {
  display.setPixel(cx - 1, cy);
  display.setPixel(cx, cy);
  display.setPixel(cx + 1, cy);
}

void drawTinyOpen(int cx, int cy) {
  display.drawRect(cx - 1, cy, 3, 2);
}

void drawEyeStyle(EyeStyle style, int cx, int cy, int gazeX, int gazeY) {
  switch (style) {
    case EYE_GLOSSY_OPEN:
      drawGlossyOpenEye(cx, cy, gazeX, gazeY);
      break;
    case EYE_SOFT_HALF:
      drawSoftHalfEye(cx, cy);
      break;
    case EYE_SOFT_CLOSED:
      drawSoftClosedEye(cx, cy);
      break;
  }
}

void drawFaceOnly(VisualState state, uint32_t t) {
  // Animations plus fluides (sinusoïdes)
  const float tf  = t * 0.001f;
  const int bob   = (int)roundf(sinf(tf * 2.0f) * 1.6f);
  const int gazeX = (int)roundf(sinf(tf * 0.9f) * 1.0f);
  const int gazeY = (int)roundf(cosf(tf * 0.7f) * 0.8f);

  // Blink adouci : ouverture -> demi -> fermé -> demi -> ouverture
  const uint32_t blinkCycle = t % 3600;
  EyeStyle eyeStyle = EYE_GLOSSY_OPEN;

  auto blendBlinkStyle = [](uint32_t cycle, uint32_t start, uint32_t duration) -> EyeStyle {
    if (cycle < start || cycle >= (start + duration)) {
      return EYE_GLOSSY_OPEN;
    }
    const float p = (float)(cycle - start) / (float)duration;         // 0..1
    const float closeAmount = 1.0f - fabsf(2.0f * p - 1.0f);          // 0..1..0
    if (closeAmount > 0.70f) return EYE_SOFT_CLOSED;
    if (closeAmount > 0.35f) return EYE_SOFT_HALF;
    return EYE_GLOSSY_OPEN;
  };

  EyeStyle b1 = blendBlinkStyle(blinkCycle, 1050, 260);
  EyeStyle b2 = blendBlinkStyle(blinkCycle, 1460, 180);
  if (b1 != EYE_GLOSSY_OPEN) eyeStyle = b1;
  if (b2 != EYE_GLOSSY_OPEN) eyeStyle = b2;

  const int leftEyeX  = 20;
  const int rightEyeX = 44;
  const int eyeY      = 18 + bob;
  const int mouthX    = 32;
  const int mouthY    = 31 + bob;

  display.clear();

  switch (state) {
    case STATE_DRY:
      drawEyeStyle(EYE_SOFT_HALF, leftEyeX, eyeY + 1, 0, 0);
      drawEyeStyle(EYE_SOFT_HALF, rightEyeX, eyeY + 1, 0, 0);
      drawTinyFlat(mouthX, mouthY + 1);
      break;

    case STATE_LOW_LIGHT:
      drawEyeStyle(EYE_SOFT_CLOSED, leftEyeX, eyeY + 1, 0, 0);
      drawEyeStyle(EYE_SOFT_CLOSED, rightEyeX, eyeY + 1, 0, 0);
      drawTinyFlat(mouthX, mouthY + 1);
      break;

    case STATE_OKAY:
      drawEyeStyle(eyeStyle, leftEyeX, eyeY, gazeX, gazeY);
      drawEyeStyle(eyeStyle, rightEyeX, eyeY, gazeX, gazeY);
      drawTinyFlat(mouthX, mouthY + 1);
      break;

    case STATE_GOOD:
      drawEyeStyle(EYE_GLOSSY_OPEN, leftEyeX, eyeY, gazeX, 0);
      drawEyeStyle(EYE_GLOSSY_OPEN, rightEyeX, eyeY, gazeX, 0);
      drawTinySmile(mouthX, mouthY + 1);
      break;

    case STATE_EXCELLENT:
      drawEyeStyle(EYE_GLOSSY_OPEN, leftEyeX, eyeY, 0, -1);
      drawEyeStyle(EYE_GLOSSY_OPEN, rightEyeX, eyeY, 0, -1);
      drawTinyOpen(mouthX, mouthY + 1);
      break;
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  display.init();
  display.flipScreenVertically();
  display.clear();
  display.display();
}

void loop() {
  const uint32_t t = millis();

  // Démo automatique des états visuels
  // Plus tard tu remplaceras ça par les vraies valeurs capteur
  const uint32_t cycle = t % 15000;
  VisualState state = STATE_OKAY;

  if (cycle < 3000) {
    state = STATE_DRY;
  } else if (cycle < 6000) {
    state = STATE_LOW_LIGHT;
  } else if (cycle < 9000) {
    state = STATE_OKAY;
  } else if (cycle < 12000) {
    state = STATE_GOOD;
  } else {
    state = STATE_EXCELLENT;
  }

  drawFaceOnly(state, t);
  delay(30);
}
