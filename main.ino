#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "app_state.h"
#include "buttons.h"
#include "can_manager.h"
#include "config.h"
#include "display.h"
#include "master_profiles.h"
#include "ui.h"

static AppState gState;
static Buttons gButtons;
static Display gDisplay;
static CanManager gCan;

static void logFrame(const __FlashStringHelper* tag, const CanFrame& frame) {
  Serial.print(tag);
  Serial.print(F(" id=0x"));
  Serial.print(frame.id, HEX);
  Serial.print(F(" dlc="));
  Serial.print(frame.dlc);
  Serial.print(F(" data="));

  for (uint8_t i = 0; i < frame.dlc && i < 8; ++i) {
    if (frame.data[i] < 0x10) {
      Serial.print('0');
    }
    Serial.print(frame.data[i], HEX);
    if (i + 1 < frame.dlc) {
      Serial.write(' ');
    }
  }
  Serial.println();
}

static void sendMasterFrames(uint32_t nowMs) {
  uint8_t profileCount = 0;
  const MasterProfile* profiles = masterGetProfiles(profileCount);
  if (profileCount == 0) {
    return;
  }

  const MasterProfile& p = profiles[gState.masterProfileIndex % profileCount];
  if (!p.enabled || p.frameCount == 0) {
    return;
  }

  if ((nowMs - gState.masterLastTxAtMs) < p.periodMs) {
    return;
  }

  gState.masterLastTxAtMs = nowMs;

  const uint8_t frameIdx = gState.masterNextFrameIndex;
  const CanTxFrame& tx = p.frames[frameIdx];

  CanFrame frame;
  frame.id = tx.id;
  frame.dlc = tx.dlc;
  for (uint8_t i = 0; i < 8; ++i) {
    frame.data[i] = tx.data[i];
  }

  if (canSend(gCan, frame)) {
    gState.masterTxCount++;
    gState.masterLastTxId = frame.id;
    logFrame(F("[CAN] TX"), frame);
    displayMarkDirty(gDisplay);
  } else {
    Serial.println(F("[CAN] TX failed"));
  }

  gState.masterNextFrameIndex++;
  if (gState.masterNextFrameIndex >= p.frameCount) {
    gState.masterNextFrameIndex = p.cyclic ? 0 : (p.frameCount - 1);
  }
}

void setup() {
  const uint32_t nowMs = millis();
  appStateInit(gState, nowMs);

  Serial.begin(config::SERIAL_BAUD);
  Serial.println(F("[SYS] Boot start"));

  SPI.begin();
  Wire.begin();

  gState.displayReady = displayInit(gDisplay);
  Serial.println(gState.displayReady ? F("[SYS] OLED init OK") : F("[SYS] OLED init FAIL"));

  buttonsInit(gButtons);
  gState.buttonsReady = true;
  Serial.println(F("[SYS] Buttons init OK"));

  gState.canReady = true;

  gState.bootShown = true;
  displayMarkDirty(gDisplay);

  if (gState.displayReady && gState.buttonsReady) {
    Serial.println(F("[SYS] Modules initialized"));
  } else {
    Serial.println(F("[SYS] Init finished with warnings"));
  }
}

void loop() {
  const uint32_t nowMs = millis();

  uiTick(gState, nowMs);

  const ButtonEvent event = buttonsPoll(gButtons, nowMs);
  if (event != BTN_EVENT_NONE) {
    if (uiHandleButton(gState, event, nowMs)) {
      displayMarkDirty(gDisplay);
      Serial.print(F("[UI] Button event="));
      Serial.println(static_cast<uint8_t>(event));
    }
  }

  if (gState.requestSnifferStart) {
    gState.requestSnifferStart = false;
    const config::SnifferSpeed speed = static_cast<config::SnifferSpeed>(gState.snifferSpeedIndex);

    if (canInitSniffer(gCan, speed)) {
      gState.view = config::VIEW_SNIFFER_RUN;
      gState.lastCanPollMs = 0;
      gState.snifferFrameCount = 0;
      gState.lastCanId = 0;
      gState.lastCanDlc = 0;
      Serial.print(F("[UI] Sniffer start at "));
      Serial.println(canSpeedLabel(speed));
    } else {
      Serial.println(F("[UI] Sniffer start failed"));
    }
    displayMarkDirty(gDisplay);
  }

  if (gState.requestSnifferStop) {
    gState.requestSnifferStop = false;
    canStop(gCan);
    Serial.println(F("[UI] Sniffer stopped"));
    displayMarkDirty(gDisplay);
  }

  if (gState.requestMasterStart) {
    gState.requestMasterStart = false;
    uint8_t profileCount = 0;
    const MasterProfile* profiles = masterGetProfiles(profileCount);
    if (profileCount > 0) {
      const MasterProfile& p = profiles[gState.masterProfileIndex % profileCount];
      if (canInitMaster(gCan, p.speed)) {
        gState.view = config::VIEW_MASTER_RUN;
        gState.masterTxCount = 0;
        gState.masterLastTxId = 0;
        gState.masterLastTxAtMs = 0;
        gState.masterNextFrameIndex = 0;
        gState.lastCanPollMs = 0;
        Serial.print(F("[UI] Master start: "));
        Serial.println(reinterpret_cast<const __FlashStringHelper*>(p.name));
      } else {
        Serial.println(F("[UI] Master start failed"));
      }
    }
    displayMarkDirty(gDisplay);
  }

  if (gState.requestMasterStop) {
    gState.requestMasterStop = false;
    canStop(gCan);
    Serial.println(F("[UI] Master stopped"));
    displayMarkDirty(gDisplay);
  }

  if (gState.view == config::VIEW_SNIFFER_RUN) {
    CanFrame frame;
    if (canPoll(gCan, nowMs, gState.lastCanPollMs, frame)) {
      gState.snifferFrameCount++;
      gState.lastCanId = frame.id;
      gState.lastCanDlc = frame.dlc;
      for (uint8_t i = 0; i < 8; ++i) {
        gState.lastCanData[i] = frame.data[i];
      }
      logFrame(F("[CAN] RX"), frame);
      displayMarkDirty(gDisplay);
    }
  }

  if (gState.view == config::VIEW_MASTER_RUN) {
    sendMasterFrames(nowMs);

    CanFrame frame;
    if (canPoll(gCan, nowMs, gState.lastCanPollMs, frame)) {
      logFrame(F("[CAN] RX"), frame);
    }
  }

  static config::View prevView = config::VIEW_BOOT;
  if (prevView != gState.view) {
    prevView = gState.view;
    displayMarkDirty(gDisplay);
    Serial.print(F("[UI] View changed="));
    Serial.println(static_cast<uint8_t>(gState.view));
  }

  if (gState.displayReady) {
    displayUpdate(gDisplay, gState, nowMs);
  }
}
