#include "display.h"

#include "can_manager.h"
#include "config.h"
#include "master_profiles.h"

Display::Display()
    : oled(config::OLED_WIDTH, config::OLED_HEIGHT, &Wire, -1), dirty(true), lastDrawMs(0) {}

static void drawTitle(Adafruit_SSD1306& oled, const __FlashStringHelper* text) {
  oled.setTextSize(2);
  oled.setCursor(0, 0);
  oled.print(text);
  oled.setTextSize(1);
}

static void drawMainMenu(Adafruit_SSD1306& oled, uint8_t selected) {
  drawTitle(oled, F("MENU"));

  oled.setCursor(0, 26);
  oled.print(selected == config::MENU_SNIFFER ? F("> Snifer") : F("  Snifer"));
  oled.setCursor(0, 42);
  oled.print(selected == config::MENU_MASTER ? F("> Master") : F("  Master"));

  oled.setCursor(0, 56);
  oled.print(F("UP/DN  SEL"));
}

static const __FlashStringHelper* speedNameRu(config::SnifferSpeed speed) {
  switch (speed) {
    case config::SNIFFER_SPEED_125: return F("125 kbps");
    case config::SNIFFER_SPEED_250: return F("250 kbps");
    case config::SNIFFER_SPEED_500:
    default:
      return F("500 kbps");
  }
}

static void drawSnifferSpeed(Adafruit_SSD1306& oled, uint8_t selected) {
  drawTitle(oled, F("SKOROST"));

  oled.setCursor(0, 30);
  oled.setTextSize(2);
  oled.print(speedNameRu(static_cast<config::SnifferSpeed>(selected)));
  oled.setTextSize(1);

  oled.setCursor(0, 56);
  oled.print(F("UP/DN SEL=Pusk"));
}

static void drawSnifferRun(Adafruit_SSD1306& oled, const AppState& state) {
  drawTitle(oled, F("SNIFER"));

  oled.setCursor(0, 22);
  oled.print(F("Skor: "));
  oled.print(speedNameRu(static_cast<config::SnifferSpeed>(state.snifferSpeedIndex)));

  oled.setCursor(0, 34);
  oled.print(F("Kadry: "));
  oled.print(state.snifferFrameCount);

  oled.setCursor(0, 46);
  oled.print(F("ID: 0x"));
  oled.print(state.lastCanId, HEX);

  oled.setCursor(0, 56);
  oled.print(F("BACK=Nazad"));
}

static void drawMasterList(Adafruit_SSD1306& oled, uint8_t selected) {
  uint8_t count = 0;
  const MasterProfile* profiles = masterGetProfiles(count);
  drawTitle(oled, F("MASTER"));

  if (count == 0) {
    oled.setCursor(0, 32);
    oled.print(F("Net profiley"));
    return;
  }

  oled.setCursor(0, 24);
  oled.print(F("Profil:"));
  oled.setCursor(0, 38);
  oled.print(reinterpret_cast<const __FlashStringHelper*>(profiles[selected % count].name));

  oled.setCursor(0, 56);
  oled.print(F("UP/DN SEL=Pusk"));
}

static void drawMasterRun(Adafruit_SSD1306& oled, const AppState& state) {
  uint8_t count = 0;
  const MasterProfile* profiles = masterGetProfiles(count);
  drawTitle(oled, F("MASTER"));

  if (count == 0) {
    oled.setCursor(0, 32);
    oled.print(F("Net profilya"));
    return;
  }

  const MasterProfile& p = profiles[state.masterProfileIndex % count];

  oled.setCursor(0, 22);
  oled.print(reinterpret_cast<const __FlashStringHelper*>(p.name));

  oled.setCursor(0, 34);
  oled.print(F("Skor: "));
  oled.print(speedNameRu(p.speed));

  oled.setCursor(0, 46);
  oled.print(F("TX: "));
  oled.print(state.masterTxCount);
  oled.print(F("  ID:0x"));
  oled.print(state.masterLastTxId, HEX);

  oled.setCursor(0, 56);
  oled.print(F("BACK=Stop"));
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
  display.oled.setTextSize(1);
  display.oled.setCursor(0, 0);

  switch (state.view) {
    case config::VIEW_BOOT:
      display.oled.setCursor(0, 0);
      display.oled.print(F("FeRgAnI Studio"));
      display.oled.setCursor(0, 20);
      display.oled.print(F("CAN BMW Tool"));
      display.oled.setCursor(0, 40);
      display.oled.print(F("Zapusk..."));
      break;

    case config::VIEW_STARTUP_OK:
      display.oled.setCursor(0, 0);
      display.oled.print(F("FeRgAnI Studio"));
      display.oled.setCursor(0, 26);
      display.oled.setTextSize(2);
      display.oled.print(F("GOTOV"));
      display.oled.setTextSize(1);
      display.oled.setCursor(0, 52);
      display.oled.print(F("Modul gotov"));
      break;

    case config::VIEW_ERROR:
      display.oled.setCursor(0, 0);
      display.oled.print(F("FeRgAnI Studio"));
      display.oled.setCursor(0, 20);
      display.oled.setTextSize(2);
      display.oled.print(F("CAN ERROR"));
      display.oled.setTextSize(1);
      display.oled.setCursor(0, 52);
      display.oled.print(F("Proverte modul"));
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
