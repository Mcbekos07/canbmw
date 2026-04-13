#include "display.h"

#include "can_manager.h"
#include "config.h"
#include "master_profiles.h"

Display::Display()
    : oled(config::OLED_WIDTH, config::OLED_HEIGHT, &Wire, -1), dirty(true), lastDrawMs(0) {}

static void drawMainMenu(Adafruit_SSD1306& oled, uint8_t selected) {
  oled.setCursor(0, 0);
  oled.print(F("Главное меню"));

  oled.setCursor(0, 18);
  oled.print(selected == config::MENU_SNIFFER ? F("> Снифер") : F("  Снифер"));

  oled.setCursor(0, 34);
  oled.print(selected == config::MENU_MASTER ? F("> Master") : F("  Master"));

  oled.setCursor(0, 54);
  oled.print(F("UP/DN SEL BACK"));
}

static void drawSnifferSpeed(Adafruit_SSD1306& oled, uint8_t selected) {
  oled.setCursor(0, 0);
  oled.print(F("Скорость CAN"));

  oled.setCursor(0, 16);
  oled.print(selected == config::SNIFFER_SPEED_125 ? F("> 125 kbps") : F("  125 kbps"));

  oled.setCursor(0, 30);
  oled.print(selected == config::SNIFFER_SPEED_250 ? F("> 250 kbps") : F("  250 kbps"));

  oled.setCursor(0, 44);
  oled.print(selected == config::SNIFFER_SPEED_500 ? F("> 500 kbps") : F("  500 kbps"));

  oled.setCursor(0, 56);
  oled.print(F("SEL=Пуск BACK"));
}

static char toHex(uint8_t v) {
  return (v < 10) ? static_cast<char>('0' + v) : static_cast<char>('A' + (v - 10));
}

static void byteToHex(uint8_t value, char* out2) {
  out2[0] = toHex((value >> 4) & 0x0F);
  out2[1] = toHex(value & 0x0F);
}

static void drawSnifferRun(Adafruit_SSD1306& oled, const AppState& state) {
  oled.setCursor(0, 0);
  oled.print(F("Снифер "));
  oled.print(canSpeedLabel(static_cast<config::SnifferSpeed>(state.snifferSpeedIndex)));

  oled.setCursor(0, 12);
  oled.print(F("Cnt: "));
  oled.print(state.snifferFrameCount);

  oled.setCursor(0, 24);
  oled.print(F("ID:0x"));
  oled.print(state.lastCanId, HEX);

  oled.setCursor(80, 24);
  oled.print(F("DLC:"));
  oled.print(state.lastCanDlc);

  oled.setCursor(0, 38);
  oled.print(F("DATA:"));

  char hex2[2];
  uint8_t x = 34;
  for (uint8_t i = 0; i < state.lastCanDlc && i < 8; ++i) {
    byteToHex(state.lastCanData[i], hex2);
    oled.setCursor(x, 38);
    oled.write(hex2[0]);
    oled.write(hex2[1]);
    x = static_cast<uint8_t>(x + 15);
  }

  oled.setCursor(0, 56);
  oled.print(F("BACK=скорость"));
}

static void drawMasterList(Adafruit_SSD1306& oled, uint8_t selected) {
  uint8_t count = 0;
  const MasterProfile* profiles = masterGetProfiles(count);
  if (count == 0) {
    oled.setCursor(0, 0);
    oled.print(F("Нет профилей"));
    return;
  }

  oled.setCursor(0, 0);
  oled.print(F("Профили Master"));

  for (uint8_t row = 0; row < 3; ++row) {
    const uint8_t idx = (selected + row) % count;
    oled.setCursor(0, static_cast<int16_t>(14 + row * 14));
    oled.print(row == 0 ? F("> ") : F("  "));
    oled.print(reinterpret_cast<const __FlashStringHelper*>(profiles[idx].name));
  }

  oled.setCursor(0, 56);
  oled.print(F("SEL=Пуск BACK"));
}

static void drawMasterRun(Adafruit_SSD1306& oled, const AppState& state) {
  uint8_t count = 0;
  const MasterProfile* profiles = masterGetProfiles(count);
  if (count == 0) {
    oled.setCursor(0, 0);
    oled.print(F("Нет профиля"));
    return;
  }
  const MasterProfile& p = profiles[state.masterProfileIndex % count];

  oled.setCursor(0, 0);
  oled.print(reinterpret_cast<const __FlashStringHelper*>(p.name));

  oled.setCursor(0, 12);
  oled.print(F("Speed: "));
  oled.print(canSpeedLabel(p.speed));

  oled.setCursor(0, 24);
  oled.print(F("Status: TX"));
  oled.print(p.cyclic ? F(" cyclic") : F(" one-shot"));

  oled.setCursor(0, 36);
  oled.print(F("Count: "));
  oled.print(state.masterTxCount);

  oled.setCursor(0, 48);
  oled.print(F("LastTX:0x"));
  oled.print(state.masterLastTxId, HEX);

  oled.setCursor(0, 56);
  oled.print(F("BACK=stop"));
}

bool displayInit(Display& display) {
  if (!display.oled.begin(SSD1306_SWITCHCAPVCC, config::OLED_I2C_ADDRESS)) {
    return false;
  }

  display.oled.clearDisplay();
  display.oled.setTextSize(1);
  display.oled.setTextColor(SSD1306_WHITE);
  display.oled.setCursor(0, 0);
  display.oled.print(F("Booting..."));
  display.oled.display();
  display.dirty = true;
  display.lastDrawMs = 0;
  return true;
}

void displayMarkDirty(Display& display) {
  display.dirty = true;
}

void displayUpdate(Display& display, const AppState& state, uint32_t nowMs) {
  if (!display.dirty) {
    return;
  }
  if ((nowMs - display.lastDrawMs) < config::DISPLAY_REFRESH_MS) {
    return;
  }

  display.oled.clearDisplay();
  display.oled.setCursor(0, 0);

  switch (state.view) {
    case config::VIEW_BOOT:
      display.oled.print(F("FeRgAnI Studio"));
      display.oled.setCursor(0, 20);
      display.oled.print(F("CAN BMW Tool"));
      display.oled.setCursor(0, 40);
      display.oled.print(F("Инициализация..."));
      break;

    case config::VIEW_STARTUP_OK:
      display.oled.print(F("FeRgAnI Studio"));
      display.oled.setCursor(0, 24);
      display.oled.print(F("Модуль готов"));
      break;

    case config::VIEW_ERROR:
      display.oled.print(F("FeRgAnI Studio"));
      display.oled.setCursor(0, 18);
      display.oled.print(F("Ошибка MCP2515"));
      display.oled.setCursor(0, 34);
      display.oled.print(F("Проверьте CAN модуль"));
      display.oled.setCursor(0, 54);
      display.oled.print(F("Перезапуск устройства"));
      break;

    case config::VIEW_MAIN_MENU:
      drawMainMenu(display.oled, state.menuIndex);
      break;

    case config::VIEW_SNIFFER_SPEED:
      drawSnifferSpeed(display.oled, state.snifferSpeedIndex);
      break;

    case config::VIEW_SNIFFER_RUN:
      drawSnifferRun(display.oled, state);
      break;

    case config::VIEW_MASTER_LIST:
      drawMasterList(display.oled, state.masterProfileIndex);
      break;

    case config::VIEW_MASTER_RUN:
      drawMasterRun(display.oled, state);
      break;
  }

  display.oled.display();
  display.lastDrawMs = nowMs;
  display.dirty = false;
}
