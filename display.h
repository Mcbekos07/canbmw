#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "app_state.h"

struct Display {
  Adafruit_SSD1306 oled;
  bool dirty;
  uint32_t lastDrawMs;

  Display();
};

bool displayInit(Display& display);
void displayMarkDirty(Display& display);
void displayUpdate(Display& display, const AppState& state, uint32_t nowMs);

#endif
