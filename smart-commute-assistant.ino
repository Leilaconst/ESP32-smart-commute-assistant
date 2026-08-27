#include "display.h"
#include "graphics.h"
#include "backend.h"

unsigned long lastPollTime = 0;
const unsigned long POLL_INTERVAL_MS = 5UL * 60UL * 1000UL; // 5 minutes

void setup() {
  Serial.begin(115200);
  connectWifi();

  if (parseJson(httpRequest())) {
    initDisplay();
  }
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastPollTime >= POLL_INTERVAL_MS) {
    lastPollTime = currentTime;

    if (parseJson(httpRequest())) {
      tft.fillScreen(TFT_WHITE);
      writeDisplay();
    }
  }
}