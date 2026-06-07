#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GC9A01A.h>
#include <Adafruit_BME280.h>

#define TFT_CS     10
#define TFT_DC      9
#define TFT_RST     8
#define BME_SDA     4
#define BME_SCL     5
#define BUTTON_PIN  6

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);
Adafruit_BME280 bme;

enum Screen { MAIN, TEMP_DETAIL, HUMID_DETAIL, PRESS_DETAIL };
Screen currentScreen = MAIN;

void drawCentered(const char* text, int y, uint8_t size, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, y);
  tft.print(text);
}

void drawMain() {
  tft.fillScreen(GC9A01A_BLACK);
  char buf[20];
  uint16_t grey = tft.color565(130, 130, 130);

  drawCentered("TEMPERATURE",   55, 1, grey);
  sprintf(buf, "%.1f C", bme.readTemperature());
  drawCentered(buf,             68, 2, tft.color565(255, 100, 100));

  drawCentered("HUMIDITY",     105, 1, grey);
  sprintf(buf, "%.1f %%", bme.readHumidity());
  drawCentered(buf,            118, 2, tft.color565(100, 150, 255));

  drawCentered("PRESSURE",     153, 1, grey);
  sprintf(buf, "%.0f hPa", bme.readPressure() / 100.0);
  drawCentered(buf,            166, 2, tft.color565(100, 220, 100));
}

void drawDetail(const char* label, float value, const char* unit, uint16_t color) {
  tft.fillScreen(GC9A01A_BLACK);
  char buf[20];
  uint16_t grey = tft.color565(130, 130, 130);

  drawCentered(label,       70, 1, grey);
  sprintf(buf, "%.1f", value);
  drawCentered(buf,         95, 4, color);
  drawCentered(unit,       148, 2, grey);
  drawCentered("[ graph ]",175, 1, tft.color565(60, 60, 60));
}

void refreshValues() {
  char buf[20];
  switch (currentScreen) {
    case MAIN:
      tft.fillRect(0, 65, 240, 20, GC9A01A_BLACK);
      sprintf(buf, "%.1f C", bme.readTemperature());
      drawCentered(buf, 68, 2, tft.color565(255, 100, 100));

      tft.fillRect(0, 115, 240, 20, GC9A01A_BLACK);
      sprintf(buf, "%.1f %%", bme.readHumidity());
      drawCentered(buf, 118, 2, tft.color565(100, 150, 255));

      tft.fillRect(0, 163, 240, 20, GC9A01A_BLACK);
      sprintf(buf, "%.0f hPa", bme.readPressure() / 100.0);
      drawCentered(buf, 166, 2, tft.color565(100, 220, 100));
      break;

    case TEMP_DETAIL:
      tft.fillRect(0, 92, 240, 36, GC9A01A_BLACK);
      sprintf(buf, "%.1f", bme.readTemperature());
      drawCentered(buf, 95, 4, tft.color565(255, 100, 100));
      break;

    case HUMID_DETAIL:
      tft.fillRect(0, 92, 240, 36, GC9A01A_BLACK);
      sprintf(buf, "%.1f", bme.readHumidity());
      drawCentered(buf, 95, 4, tft.color565(100, 150, 255));
      break;

    case PRESS_DETAIL:
      tft.fillRect(0, 92, 240, 36, GC9A01A_BLACK);
      sprintf(buf, "%.0f", bme.readPressure() / 100.0);
      drawCentered(buf, 95, 4, tft.color565(100, 220, 100));
      break;
  }
}

void drawScreen() {
  switch (currentScreen) {
    case MAIN:        drawMain();  break;
    case TEMP_DETAIL: drawDetail("TEMPERATURE", bme.readTemperature(),      "C",   tft.color565(255, 100, 100)); break;
    case HUMID_DETAIL:drawDetail("HUMIDITY",    bme.readHumidity(),         "%",   tft.color565(100, 150, 255)); break;
    case PRESS_DETAIL:drawDetail("PRESSURE",    bme.readPressure() / 100.0, "hPa", tft.color565(100, 220, 100)); break;
  }
}

void checkButton() {
  static bool lastState = HIGH;
  static unsigned long lastPress = 0;

  bool state = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  if (state == LOW && lastState == HIGH && now - lastPress > 200) {
    lastPress = now;
    currentScreen = (Screen)((currentScreen + 1) % 4);
    drawScreen();
  }

  lastState = state;
}

void setup() {
  tft.begin();
  tft.setRotation(2);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Wire.begin(BME_SDA, BME_SCL);
  if (!bme.begin(0x76)) {
    tft.fillScreen(GC9A01A_BLACK);
    drawCentered("BME280",     100, 2, GC9A01A_RED);
    drawCentered("not found!", 125, 2, GC9A01A_RED);
    while (true);
  }

  drawScreen();
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();

  checkButton();

  if (now - lastUpdate >= 2000) {
    lastUpdate = now;
    refreshValues();
  }
}
