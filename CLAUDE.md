# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

ESP32-S3 microcontroller display experiments, developed as a C++ learning project. Sketches are written in Arduino-flavored C++ and compiled/uploaded via Arduino IDE — there is no CLI build system.

## Hardware

- **Board:** Freenove ESP32-S3 WROOM
- **Display:** Round 240×240 TFT, GC9A01 driver (no BLK pin)
- **Library:** Adafruit GC9A01A (hardware SPI)

### Wiring (fixed — do not change pin assignments without updating all sketches)

| Display | GPIO |
|---------|------|
| SCL     | 12   |
| SDA     | 11   |
| RST     | 8    |
| DC      | 9    |
| CS      | 10   |

GPIO11/12 are the ESP32-S3 default SPI2 (FSPI) pins, enabling hardware SPI automatically.

## Display constraints

- Pixel grid is 240×240 but the visible area is circular
- Safe drawing radius: **≤ 110px from center (120, 120)** — beyond this, content is clipped by the round bezel
- Colors use RGB565 format; use `tft.color565(r, g, b)` for custom colors

## Sketch conventions

- Use `millis()` for timing when the sketch does more than one thing concurrently; `delay()` is acceptable for single-task animations
- Center text with `getTextBounds()` to measure before placing — the default font metrics are not perfectly symmetric
- Global variables hold animation state (position, angle, trail history) that persists across `loop()` calls
- `setup()` — one-time init and static drawing; `loop()` — animation and input polling
