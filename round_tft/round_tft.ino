#include <SPI.h>
#include <Adafruit_GC9A01A.h>
#include <math.h>

#define TFT_CS    10
#define TFT_DC     9
#define TFT_RST    8

#define CENTER_X  120
#define CENTER_Y  120
#define ORBIT_R    90
#define DOT_R       6
#define TRAIL_LEN  12

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

float angle = 0;
int trailX[TRAIL_LEN];
int trailY[TRAIL_LEN];

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

  // initialize all trail positions to the starting dot position
  for (int i = 0; i < TRAIL_LEN; i++) {
    trailX[i] = CENTER_X + ORBIT_R;
    trailY[i] = CENTER_Y;
  }

  tft.setTextSize(2);
  drawCentered("I'm the cometa", GC9A01A_WHITE);
}

void loop() {
  float rad = angle * PI / 180.0;
  int x = CENTER_X + ORBIT_R * cos(rad);
  int y = CENTER_Y + ORBIT_R * sin(rad);

  // erase the oldest trail dot
  tft.fillCircle(trailX[TRAIL_LEN - 1], trailY[TRAIL_LEN - 1], DOT_R, GC9A01A_BLACK);

  // shift trail back by one
  for (int i = TRAIL_LEN - 1; i > 0; i--) {
    trailX[i] = trailX[i - 1];
    trailY[i] = trailY[i - 1];
  }

  // insert new position at front
  trailX[0] = x;
  trailY[0] = y;

  // draw trail from oldest to newest so newest is on top
  for (int i = TRAIL_LEN - 1; i >= 0; i--) {
    uint8_t brightness = (uint8_t)(255.0f * (TRAIL_LEN - i) / TRAIL_LEN);
    uint16_t color = tft.color565(brightness, 0, 0);
    tft.fillCircle(trailX[i], trailY[i], DOT_R, color);
  }

  angle += 3;
  if (angle >= 360) angle = 0;

  delay(20);
}
