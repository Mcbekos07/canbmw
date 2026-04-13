#ifndef MASTER_PROFILES_H
#define MASTER_PROFILES_H

#include <Arduino.h>

#include "config.h"

struct CanTxFrame {
  uint32_t id;
  uint8_t dlc;
  uint8_t data[8];
};

struct MasterProfile {
  const __FlashStringHelper* name;
  config::SnifferSpeed speed;
  const CanTxFrame* frames;
  uint8_t frameCount;
  uint16_t periodMs;
  bool enabled;
  bool cyclic;
};

const MasterProfile* masterGetProfiles(uint8_t& count);

#endif
