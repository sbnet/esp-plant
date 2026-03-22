#include <Arduino.h>
#include "DisplayManager.h"
#include "EmotionEngine.h"

DisplayManager gDisplay;
EmotionEngine gEmotion;

void setup() {
  Serial.begin(115200);
  delay(200);

  gDisplay.begin();
}

void loop() {
  const uint32_t now = millis();
  const VisualState state = gEmotion.stateFromDemo(now);
  gDisplay.renderFace(state, now);
  delay(30);
}
