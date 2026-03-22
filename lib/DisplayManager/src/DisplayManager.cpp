#include "DisplayManager.h"

#include <math.h>

DisplayManager::DisplayManager(uint8_t address, uint8_t sdaPin, uint8_t sclPin)
    : display_(address, sdaPin, sclPin, GEOMETRY_64_48) {}

void DisplayManager::begin() {
  display_.init();
  display_.flipScreenVertically();
  display_.clear();
  display_.display();
}

DisplayManager::EyeStyle DisplayManager::blendBlinkStyle(uint32_t cycle, uint32_t start, uint32_t duration) {
  if (cycle < start || cycle >= (start + duration)) {
    return EyeStyle::GlossyOpen;
  }

  const float p = (float)(cycle - start) / (float)duration; // 0..1
  const float closeAmount = 1.0f - fabsf(2.0f * p - 1.0f);  // 0..1..0

  if (closeAmount > 0.70f) return EyeStyle::SoftClosed;
  if (closeAmount > 0.35f) return EyeStyle::SoftHalf;
  return EyeStyle::GlossyOpen;
}

void DisplayManager::drawGlossyOpenEye(int cx, int cy, int pdx, int pdy) {
  display_.drawCircle(cx, cy, 5);
  display_.fillCircle(cx + pdx, cy + pdy, 2);

  display_.setPixel(cx - 2, cy - 2);
  display_.setPixel(cx - 1, cy - 3);
  display_.setPixel(cx + 2, cy - 2);
}

void DisplayManager::drawSoftHalfEye(int cx, int cy) {
  display_.drawLine(cx - 5, cy, cx + 5, cy);
  display_.drawLine(cx - 4, cy + 1, cx + 4, cy + 1);
}

void DisplayManager::drawSoftClosedEye(int cx, int cy) {
  display_.drawLine(cx - 5, cy + 1, cx - 1, cy);
  display_.drawLine(cx - 1, cy, cx + 1, cy);
  display_.drawLine(cx + 1, cy, cx + 5, cy + 1);
}

void DisplayManager::drawTinySmile(int cx, int cy) {
  display_.setPixel(cx - 2, cy);
  display_.setPixel(cx - 1, cy + 1);
  display_.setPixel(cx, cy + 1);
  display_.setPixel(cx + 1, cy + 1);
  display_.setPixel(cx + 2, cy);
}

void DisplayManager::drawTinyFlat(int cx, int cy) {
  display_.setPixel(cx - 1, cy);
  display_.setPixel(cx, cy);
  display_.setPixel(cx + 1, cy);
}

void DisplayManager::drawTinyOpen(int cx, int cy) {
  display_.drawRect(cx - 1, cy, 3, 2);
}

void DisplayManager::drawEyeStyle(EyeStyle style, int cx, int cy, int gazeX, int gazeY) {
  switch (style) {
    case EyeStyle::GlossyOpen:
      drawGlossyOpenEye(cx, cy, gazeX, gazeY);
      break;
    case EyeStyle::SoftHalf:
      drawSoftHalfEye(cx, cy);
      break;
    case EyeStyle::SoftClosed:
      drawSoftClosedEye(cx, cy);
      break;
  }
}

void DisplayManager::renderFace(VisualState state, uint32_t tMillis) {
  const float tf = tMillis * 0.001f;
  const int bob = (int)roundf(sinf(tf * 2.0f) * 1.6f);
  const int gazeX = (int)roundf(sinf(tf * 0.9f) * 1.0f);
  const int gazeY = (int)roundf(cosf(tf * 0.7f) * 0.8f);

  const uint32_t blinkCycle = tMillis % 3600;
  EyeStyle eyeStyle = EyeStyle::GlossyOpen;
  EyeStyle b1 = blendBlinkStyle(blinkCycle, 1050, 260);
  EyeStyle b2 = blendBlinkStyle(blinkCycle, 1460, 180);
  if (b1 != EyeStyle::GlossyOpen) eyeStyle = b1;
  if (b2 != EyeStyle::GlossyOpen) eyeStyle = b2;

  const int leftEyeX = 20;
  const int rightEyeX = 44;
  const int eyeY = 18 + bob;
  const int mouthX = 32;
  const int mouthY = 31 + bob;

  display_.clear();

  switch (state) {
    case VisualState::Dry:
      drawEyeStyle(EyeStyle::SoftHalf, leftEyeX, eyeY + 1, 0, 0);
      drawEyeStyle(EyeStyle::SoftHalf, rightEyeX, eyeY + 1, 0, 0);
      drawTinyFlat(mouthX, mouthY + 1);
      break;

    case VisualState::LowLight:
      drawEyeStyle(EyeStyle::SoftClosed, leftEyeX, eyeY + 1, 0, 0);
      drawEyeStyle(EyeStyle::SoftClosed, rightEyeX, eyeY + 1, 0, 0);
      drawTinyFlat(mouthX, mouthY + 1);
      break;

    case VisualState::Okay:
      drawEyeStyle(eyeStyle, leftEyeX, eyeY, gazeX, gazeY);
      drawEyeStyle(eyeStyle, rightEyeX, eyeY, gazeX, gazeY);
      drawTinyFlat(mouthX, mouthY + 1);
      break;

    case VisualState::Good:
      drawEyeStyle(EyeStyle::GlossyOpen, leftEyeX, eyeY, gazeX, 0);
      drawEyeStyle(EyeStyle::GlossyOpen, rightEyeX, eyeY, gazeX, 0);
      drawTinySmile(mouthX, mouthY + 1);
      break;

    case VisualState::Excellent:
      drawEyeStyle(EyeStyle::GlossyOpen, leftEyeX, eyeY, 0, -1);
      drawEyeStyle(EyeStyle::GlossyOpen, rightEyeX, eyeY, 0, -1);
      drawTinyOpen(mouthX, mouthY + 1);
      break;
  }

  display_.display();
}

