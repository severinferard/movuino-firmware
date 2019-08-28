#ifndef _MOVUINO_NEOPIXEL_H_
#define _MOVUINO_NEOPIXEL_H_

#include <NeoPixelBus.h>
#include "globals.h"

class NeoPixel {
private:
  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *pix;
  RgbColor c;

public:
  NeoPixel() {
    pix = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(1, NEOPIXEL_PIN);
  }
  
  ~NeoPixel() {
    delete pix;
  }

  void init() {
    pix->Begin();
    pix->Show();
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b) {
    c = RgbColor(r, g, b);
  }

  void update() {
    pix->SetPixelColor(0, c);
    pix->Show();
  }
};

#endif /* _MOVUINO_NEOPIXEL_H_ */
