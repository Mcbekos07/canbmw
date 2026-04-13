#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include <Arduino.h>
#include <mcp_can.h>

#include "config.h"

struct CanFrame {
  uint32_t id;
  uint8_t dlc;
  uint8_t data[8];
};

struct CanManager {
  MCP_CAN controller;
  bool ready;
  config::SnifferSpeed speed;

  CanManager();
};

bool canInitSniffer(CanManager& can, config::SnifferSpeed speed);
bool canInitMaster(CanManager& can, config::SnifferSpeed speed);
bool canProbe(CanManager& can);
void canStop(CanManager& can);
bool canPoll(CanManager& can, uint32_t nowMs, uint32_t& lastPollMs, CanFrame& outFrame);
bool canSend(CanManager& can, const CanFrame& frame);
const __FlashStringHelper* canSpeedLabel(config::SnifferSpeed speed);

#endif
