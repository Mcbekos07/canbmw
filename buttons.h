#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

enum ButtonEvent : uint8_t {
  BTN_EVENT_NONE = 0,
  BTN_EVENT_UP,
  BTN_EVENT_DOWN,
  BTN_EVENT_SELECT,
  BTN_EVENT_BACK
};

struct Buttons {
  uint8_t pins[4];
  uint8_t stableState[4];
  uint8_t lastReadState[4];
  uint32_t changedAtMs[4];
};

void buttonsInit(Buttons& buttons);
ButtonEvent buttonsPoll(Buttons& buttons, uint32_t nowMs);

#endif
