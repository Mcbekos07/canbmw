#include "ui.h"

#include "config.h"
#include "master_profiles.h"

bool uiHandleButton(AppState& state, ButtonEvent event, uint32_t nowMs) {
  (void)nowMs;
  if (event == BTN_EVENT_NONE) {
    return false;
  }

  uint8_t profileCount = 0;
  masterGetProfiles(profileCount);

  switch (state.view) {
    case config::VIEW_BOOT:
      return false;

    case config::VIEW_MAIN_MENU:
      if (event == BTN_EVENT_UP) {
        state.menuIndex = (state.menuIndex == 0) ? (config::MENU_COUNT - 1) : (state.menuIndex - 1);
        return true;
      }
      if (event == BTN_EVENT_DOWN) {
        state.menuIndex = (state.menuIndex + 1) % config::MENU_COUNT;
        return true;
      }
      if (event == BTN_EVENT_SELECT) {
        state.view = (state.menuIndex == config::MENU_SNIFFER) ? config::VIEW_SNIFFER_SPEED : config::VIEW_MASTER_LIST;
        return true;
      }
      return false;

    case config::VIEW_SNIFFER_SPEED:
      if (event == BTN_EVENT_UP) {
        state.snifferSpeedIndex =
            (state.snifferSpeedIndex == 0) ? (config::SNIFFER_SPEED_COUNT - 1) : (state.snifferSpeedIndex - 1);
        return true;
      }
      if (event == BTN_EVENT_DOWN) {
        state.snifferSpeedIndex = (state.snifferSpeedIndex + 1) % config::SNIFFER_SPEED_COUNT;
        return true;
      }
      if (event == BTN_EVENT_SELECT) {
        state.requestSnifferStart = true;
        return true;
      }
      if (event == BTN_EVENT_BACK) {
        state.view = config::VIEW_MAIN_MENU;
        return true;
      }
      return false;

    case config::VIEW_SNIFFER_RUN:
      if (event == BTN_EVENT_BACK) {
        state.requestSnifferStop = true;
        state.view = config::VIEW_SNIFFER_SPEED;
        return true;
      }
      return false;

    case config::VIEW_MASTER_LIST:
      if (event == BTN_EVENT_UP && profileCount > 0) {
        state.masterProfileIndex = (state.masterProfileIndex == 0) ? (profileCount - 1) : (state.masterProfileIndex - 1);
        return true;
      }
      if (event == BTN_EVENT_DOWN && profileCount > 0) {
        state.masterProfileIndex = (state.masterProfileIndex + 1) % profileCount;
        return true;
      }
      if (event == BTN_EVENT_SELECT) {
        state.requestMasterStart = true;
        return true;
      }
      if (event == BTN_EVENT_BACK) {
        state.view = config::VIEW_MAIN_MENU;
        return true;
      }
      return false;

    case config::VIEW_MASTER_RUN:
      if (event == BTN_EVENT_BACK) {
        state.requestMasterStop = true;
        state.view = config::VIEW_MASTER_LIST;
        return true;
      }
      return false;
  }
  return false;
}

void uiTick(AppState& state, uint32_t nowMs) {
  if (state.view == config::VIEW_BOOT && (nowMs - state.bootStartMs) >= config::BOOT_SCREEN_MS) {
    state.view = config::VIEW_MAIN_MENU;
  }
}
