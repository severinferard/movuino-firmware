#ifndef _MOVUINO_INDICATOR_LIGHT_H_
#define _MOVUINO_INDICATOR_LIGHT_H_

#include <Arduino.h>
#include "NeoPixel.h"
#include "config.h"

class IndicatorLight {
private:
  NeoPixel *neopix;

  bool state;
  bool blinking;
  bool useNeoPixel;

  unsigned long period;
  unsigned long lastTickDate;

public:
  IndicatorLight() :
  neopix(NULL),
  state(false),
  blinking(false),
  useNeoPixel(DEFAULT_USE_NEOPIXEL_AS_INDICATOR),
  period(100) {
    lastTickDate = millis();
  }

  ~IndicatorLight() {}

  void init(NeoPixel *np) {
    neopix = np;
  }

  void setUseNeoPixel(bool use) {
    useNeoPixel = use;
  }

  void setPeriod(unsigned long p) {
    blinking = true;
    state = true;
    period = p;
    lastTickDate = millis();
  }

  void setHigh() {
    blinking = false;
    state = true;
  }

  void setLow() {
    blinking = false;
    state = false;
  }

  void update() {
    unsigned long now = millis();

    if (blinking && now - lastTickDate >= period) {
      lastTickDate = now;
      state = !state;
    }

    if (useNeoPixel) { // blink in not too bright blue
      neopix->setColor(0, 0, state ? 127 : 0);
    } else {
#ifdef ESP8266 // no simple led on esp32 version
      digitalWrite(LED_PIN, state ? LOW : HIGH);
#endif
    }
  }
};

#endif /* _MOVUINO_INDICATOR_LIGHT_H_ */