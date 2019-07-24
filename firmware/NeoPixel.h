#ifndef _MOVUINO_NEOPIXEL_H_
#define _MOVUINO_NEOPIXEL_H_

#include <Adafruit_NeoPixel.h>
#include "globals.h"

class NeoPixel {
private:
  Adafruit_NeoPixel pix = Adafruit_NeoPixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
  uint8_t red, green, blue;

public:
  NeoPixel() {}
  ~NeoPixel() {}

  void init() {
    pix.begin();
    pix.clear();
    pix.show();
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b) {
    red = r;
    green = g;
    blue = b;
  }

  void update() {
    pix.clear();
    pix.setPixelColor(0, pix.Color(red, green, blue));
    pix.show();
  }
};

#endif /* _MOVUINO_NEOPIXEL_H_ */
