#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace config {

static const uint8_t CAN_CS_PIN = 10;
static const uint8_t CAN_INT_PIN = 2;

static const uint8_t BTN_UP_PIN = 3;
static const uint8_t BTN_DOWN_PIN = 4;
static const uint8_t BTN_SELECT_PIN = 5;
static const uint8_t BTN_BACK_PIN = 6;

static const uint8_t OLED_I2C_ADDRESS = 0x3C;
static const uint16_t OLED_WIDTH = 128;
static const uint16_t OLED_HEIGHT = 64;

static const uint32_t SERIAL_BAUD = 115200UL;
static const uint16_t BOOT_SCREEN_MS = 1200;
static const uint16_t STARTUP_OK_MS = 1000;
static const uint16_t BUTTON_DEBOUNCE_MS = 25;
static const uint16_t DISPLAY_REFRESH_MS = 33;
static const uint16_t CAN_POLL_MS = 2;

enum View : uint8_t {
  VIEW_BOOT = 0,
  VIEW_STARTUP_OK,
  VIEW_ERROR,
  VIEW_MAIN_MENU,
  VIEW_SNIFFER_SPEED,
  VIEW_SNIFFER_RUN,
  VIEW_MASTER_LIST,
  VIEW_MASTER_RUN
};

enum MenuItem : uint8_t {
  MENU_SNIFFER = 0,
  MENU_MASTER,
  MENU_COUNT
};

enum SnifferSpeed : uint8_t {
  SNIFFER_SPEED_125 = 0,
  SNIFFER_SPEED_250,
  SNIFFER_SPEED_500,
  SNIFFER_SPEED_COUNT
};

}  // namespace config

#endif
