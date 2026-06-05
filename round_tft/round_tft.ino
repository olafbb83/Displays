#include <SPI.h>
#include <Adafruit_GC9A01A.h>

#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

int counter = 0;
unsigned long lastTick = 0;

void drawCentered(const char* text, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextColor(color);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, (240 - h) / 2);
  tft.print(text);
}

void setup() {
  tft.begin();
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextSize(4);
}

void loop() {
  unsigned long now = millis();

  if (now - lastTick >= 1000) {
    lastTick = now;
    counter++;

    tft.fillScreen(GC9A01A_BLACK);

    char buf[10];
    sprintf(buf, "%d", counter);
    drawCentered(buf, GC9A01A_WHITE);
  }
}
