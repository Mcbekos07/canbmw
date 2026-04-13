#include "app_state.h"

void appStateInit(AppState& state, uint32_t nowMs) {
  state.view = config::VIEW_BOOT;
  state.menuIndex = config::MENU_SNIFFER;
  state.snifferSpeedIndex = config::SNIFFER_SPEED_500;
  state.masterProfileIndex = 0;
  state.canReady = false;
  state.displayReady = false;
  state.buttonsReady = false;
  state.bootShown = false;
  state.requestSnifferStart = false;
  state.requestSnifferStop = false;
  state.requestMasterStart = false;
  state.requestMasterStop = false;
  state.bootStartMs = nowMs;
  state.lastCanPollMs = 0;
  state.snifferFrameCount = 0;
  state.lastCanId = 0;
  state.lastCanDlc = 0;
  for (uint8_t i = 0; i < 8; ++i) {
    state.lastCanData[i] = 0;
  }
  state.masterTxCount = 0;
  state.masterLastTxId = 0;
  state.masterLastTxAtMs = 0;
  state.masterNextFrameIndex = 0;
}
