#include "master_profiles.h"

static const CanTxFrame KL15_ON_FRAMES[] = {
    {0x12F, 8, {0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00}},
    {0x130, 8, {0x11, 0x22, 0x10, 0x00, 0xAA, 0x00, 0x00, 0x01}},
};

static const CanTxFrame IGNITION_2_FRAMES[] = {
    {0x12F, 8, {0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00}},
    {0x1A6, 8, {0x02, 0xF0, 0x08, 0x00, 0x10, 0x00, 0x00, 0x00}},
};

static const CanTxFrame WAKEUP_FRAMES[] = {
    {0x130, 8, {0xAA, 0x55, 0xAA, 0x55, 0x00, 0x00, 0x00, 0x00}},
};

static const CanTxFrame BMW_CLIMATE_TEST_1_FRAMES[] = {
    {0x2F1, 8, {0x40, 0x00, 0x1C, 0x0A, 0x08, 0x00, 0x00, 0x00}},
    {0x2F2, 8, {0x01, 0x01, 0x32, 0x32, 0x18, 0x00, 0x00, 0x00}},
    {0x2F3, 8, {0x00, 0x00, 0x00, 0x80, 0x55, 0x00, 0x00, 0x00}},
};

static const char PROFILE_NAME_KL15_ON[] PROGMEM = "KL15 ON";
static const char PROFILE_NAME_IGNITION_2[] PROGMEM = "Ignition 2";
static const char PROFILE_NAME_WAKEUP[] PROGMEM = "Wakeup";
static const char PROFILE_NAME_BMW_CLIMATE_TEST_1[] PROGMEM = "BMW Climate Test 1";

static const MasterProfile PROFILES[] = {
    {PROFILE_NAME_KL15_ON, config::SNIFFER_SPEED_500, KL15_ON_FRAMES,
     static_cast<uint8_t>(sizeof(KL15_ON_FRAMES) / sizeof(KL15_ON_FRAMES[0])), 100, true, true},
    {PROFILE_NAME_IGNITION_2, config::SNIFFER_SPEED_500, IGNITION_2_FRAMES,
     static_cast<uint8_t>(sizeof(IGNITION_2_FRAMES) / sizeof(IGNITION_2_FRAMES[0])), 100, true, true},
    {PROFILE_NAME_WAKEUP, config::SNIFFER_SPEED_125, WAKEUP_FRAMES,
     static_cast<uint8_t>(sizeof(WAKEUP_FRAMES) / sizeof(WAKEUP_FRAMES[0])), 50, true, true},
    {PROFILE_NAME_BMW_CLIMATE_TEST_1, config::SNIFFER_SPEED_500, BMW_CLIMATE_TEST_1_FRAMES,
     static_cast<uint8_t>(sizeof(BMW_CLIMATE_TEST_1_FRAMES) / sizeof(BMW_CLIMATE_TEST_1_FRAMES[0])), 200, true, true},
};

const MasterProfile* masterGetProfiles(uint8_t& count) {
  count = static_cast<uint8_t>(sizeof(PROFILES) / sizeof(PROFILES[0]));
  return PROFILES;
}
