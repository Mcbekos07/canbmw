#ifndef UI_H
#define UI_H

#include "app_state.h"
#include "buttons.h"

bool uiHandleButton(AppState& state, ButtonEvent event, uint32_t nowMs);
void uiTick(AppState& state, uint32_t nowMs);

#endif
