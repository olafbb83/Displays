#include <SPI.h>
#include <Adafruit_GC9A01A.h>
#include <math.h>

#define TFT_CS    10
#define TFT_DC     9
#define TFT_RST    8

#define CENTER_X  120
#define CENTER_Y  120
#define TRAIL_LEN  12

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

struct Comet {
  float angle;
  float speed;
  int orbitRadius;
  int dotRadius;
  uint8_t r, g, b;
  int trailX[TRAIL_LEN];
  int trailY[TRAIL_LEN];
};

Comet comets[2];

void initComet(Comet &c, float angle, float speed, int orbitR, int dotR, uint8_t r, uint8_t g, uint8_t b) {
  c.angle = angle;
  c.speed = speed;
  c.orbitRadius = orbitR;
  c.dotRadius = dotR;
  c.r = r;  c.g = g;  c.b = b;
  for (int i = 0; i < TRAIL_LEN; i++) {
    c.trailX[i] = CENTER_X + orbitR;
    c.trailY[i] = CENTER_Y;
  }
}

void updateComet(Comet &c) {
  float rad = c.angle * PI / 180.0;
  int x = CENTER_X + c.orbitRadius * cos(rad);
  int y = CENTER_Y + c.orbitRadius * sin(rad);

  tft.fillCircle(c.trailX[TRAIL_LEN - 1], c.trailY[TRAIL_LEN - 1], c.dotRadius + 1, GC9A01A_BLACK);

  for (int i = TRAIL_LEN - 1; i > 0; i--) {
    c.trailX[i] = c.trailX[i - 1];
    c.trailY[i] = c.trailY[i - 1];
  }
  c.trailX[0] = x;
  c.trailY[0] = y;

  for (int i = TRAIL_LEN - 1; i >= 0; i--) {
    float fade = (float)(TRAIL_LEN - i) / TRAIL_LEN;
    uint16_t color = tft.color565(c.r * fade, c.g * fade, c.b * fade);
    tft.fillCircle(c.trailX[i], c.trailY[i], c.dotRadius, color);
  }

  c.angle += c.speed;
  if (c.angle >= 360) c.angle = 0;
}

void drawTwoLineCentered(const char* line1, const char* line2, uint16_t color) {
  int16_t x1, y1;
  uint16_t w1, h1, w2, h2;
  tft.setTextColor(color);
  tft.getTextBounds(line1, 0, 0, &x1, &y1, &w1, &h1);
  tft.getTextBounds(line2, 0, 0, &x1, &y1, &w2, &h2);
  int gap = 6;
  int totalH = h1 + gap + h2;
  int startY = (240 - totalH) / 2;
  tft.setCursor((240 - w1) / 2, startY);
  tft.print(line1);
  tft.setCursor((240 - w2) / 2, startY + h1 + gap);
  tft.print(line2);
}

void setup() {
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(GC9A01A_BLACK);

  initComet(comets[0],   0, 3, 90, 6, 255,   0,   0);  // red, outer orbit
  initComet(comets[1], 180, 2, 65, 4,   0,   0, 255);  // blue, inner orbit

  tft.setTextSize(2);
  drawTwoLineCentered("BurCafe", "'CometA'", GC9A01A_WHITE);
}

void loop() {
  updateComet(comets[0]);
  updateComet(comets[1]);
  delay(20);
}
