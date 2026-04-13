#ifndef APP_STATE_H
#define APP_STATE_H

#include <Arduino.h>
#include "config.h"

struct AppState {
  config::View view;
  uint8_t menuIndex;
  uint8_t snifferSpeedIndex;
  uint8_t masterProfileIndex;
  bool canReady;
  bool displayReady;
  bool buttonsReady;
  bool bootShown;
  bool requestSnifferStart;
  bool requestSnifferStop;
  bool requestMasterStart;
  bool requestMasterStop;
  uint32_t bootStartMs;
  uint32_t lastCanPollMs;
  uint32_t snifferFrameCount;
  uint32_t lastCanId;
  uint8_t lastCanDlc;
  uint8_t lastCanData[8];
  uint32_t masterTxCount;
  uint32_t masterLastTxId;
  uint32_t masterLastTxAtMs;
  uint8_t masterNextFrameIndex;
};

void appStateInit(AppState& state, uint32_t nowMs);

#endif
