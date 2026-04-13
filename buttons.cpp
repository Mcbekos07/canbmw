#include "buttons.h"
#include "config.h"

static ButtonEvent indexToEvent(uint8_t idx) {
  switch (idx) {
    case 0: return BTN_EVENT_UP;
    case 1: return BTN_EVENT_DOWN;
    case 2: return BTN_EVENT_SELECT;
    case 3: return BTN_EVENT_BACK;
    default: return BTN_EVENT_NONE;
  }
}

void buttonsInit(Buttons& buttons) {
  buttons.pins[0] = config::BTN_UP_PIN;
  buttons.pins[1] = config::BTN_DOWN_PIN;
  buttons.pins[2] = config::BTN_SELECT_PIN;
  buttons.pins[3] = config::BTN_BACK_PIN;

  for (uint8_t i = 0; i < 4; ++i) {
    pinMode(buttons.pins[i], INPUT_PULLUP);
    const uint8_t raw = digitalRead(buttons.pins[i]);
    buttons.lastReadState[i] = raw;
    buttons.stableState[i] = raw;
    buttons.changedAtMs[i] = 0;
  }
}

ButtonEvent buttonsPoll(Buttons& buttons, uint32_t nowMs) {
  for (uint8_t i = 0; i < 4; ++i) {
    const uint8_t raw = digitalRead(buttons.pins[i]);

    if (raw != buttons.lastReadState[i]) {
      buttons.lastReadState[i] = raw;
      buttons.changedAtMs[i] = nowMs;
    }

    if ((nowMs - buttons.changedAtMs[i]) >= config::BUTTON_DEBOUNCE_MS &&
        raw != buttons.stableState[i]) {
      buttons.stableState[i] = raw;
      if (raw == LOW) {
        return indexToEvent(i);
      }
    }
  }
  return BTN_EVENT_NONE;
}
