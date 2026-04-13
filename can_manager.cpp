#include "can_manager.h"

CanManager::CanManager() : controller(config::CAN_CS_PIN), ready(false), speed(config::SNIFFER_SPEED_500) {}

static uint8_t speedToMcp(config::SnifferSpeed speed) {
  switch (speed) {
    case config::SNIFFER_SPEED_125: return CAN_125KBPS;
    case config::SNIFFER_SPEED_250: return CAN_250KBPS;
    case config::SNIFFER_SPEED_500:
    default:
      return CAN_500KBPS;
  }
}

const __FlashStringHelper* canSpeedLabel(config::SnifferSpeed speed) {
  switch (speed) {
    case config::SNIFFER_SPEED_125: return F("125k");
    case config::SNIFFER_SPEED_250: return F("250k");
    case config::SNIFFER_SPEED_500:
    default:
      return F("500k");
  }
}

static bool canInit(CanManager& can, config::SnifferSpeed speed, uint8_t mode, const __FlashStringHelper* tag) {
  pinMode(config::CAN_INT_PIN, INPUT);

  const uint8_t initResult = can.controller.begin(MCP_ANY, speedToMcp(speed), MCP_8MHZ);
  if (initResult != CAN_OK) {
    Serial.print(F("[CAN] init failed at "));
    Serial.print(canSpeedLabel(speed));
    Serial.print(F(", code="));
    Serial.println(initResult);
    can.ready = false;
    return false;
  }

  can.controller.setMode(mode);
  can.ready = true;
  can.speed = speed;

  Serial.print(F("[CAN] "));
  Serial.print(tag);
  Serial.print(F(" ready at "));
  Serial.println(canSpeedLabel(speed));
  return true;
}

bool canInitSniffer(CanManager& can, config::SnifferSpeed speed) {
  return canInit(can, speed, MCP_LISTENONLY, F("sniffer"));
}

bool canInitMaster(CanManager& can, config::SnifferSpeed speed) {
  return canInit(can, speed, MCP_NORMAL, F("master"));
}

bool canProbe(CanManager& can) {
  pinMode(config::CAN_INT_PIN, INPUT);
  const uint8_t initResult = can.controller.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);
  if (initResult != CAN_OK) {
    Serial.print(F("[CAN] probe fail, code="));
    Serial.println(initResult);
    can.ready = false;
    return false;
  }
  can.controller.setMode(MCP_SLEEP);
  can.ready = false;
  Serial.println(F("[CAN] probe OK"));
  return true;
}

void canStop(CanManager& can) {
  can.controller.setMode(MCP_SLEEP);
  can.ready = false;
}

bool canPoll(CanManager& can, uint32_t nowMs, uint32_t& lastPollMs, CanFrame& outFrame) {
  if (!can.ready) {
    return false;
  }
  if ((nowMs - lastPollMs) < config::CAN_POLL_MS) {
    return false;
  }
  lastPollMs = nowMs;

  if (digitalRead(config::CAN_INT_PIN) != LOW) {
    return false;
  }

  if (can.controller.checkReceive() != CAN_MSGAVAIL) {
    return false;
  }

  unsigned long canId = 0;
  uint8_t len = 0;
  can.controller.readMsgBuf(&canId, &len, outFrame.data);

  outFrame.id = canId;
  outFrame.dlc = len;
  return true;
}

bool canSend(CanManager& can, const CanFrame& frame) {
  if (!can.ready) {
    return false;
  }

  const uint8_t rc = can.controller.sendMsgBuf(frame.id, 0, frame.dlc, const_cast<uint8_t*>(frame.data));
  return rc == CAN_OK;
}
