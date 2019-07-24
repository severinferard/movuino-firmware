#include "ConfigFile.h"
#include "IndicatorLight"
#include "NeoPixel.h"
#include "Vibrator.h"
#include "Button.h"
#include "IntegratedSensors.h"
#include "Router.h"
#include "SerialInterface.h"
#include "WiFiInterface.h"

#include "globals.h"

ConfigFile config;
IndicatorLight indic;
NeoPixel neopix;
Vibrator vibro;
Button button;
IntegratedSensors imu;
Router router;
SerialInterface serial;
WiFiInterface wifi;

// TODO : add a SensorShield class

WiFiBootMode bootMode;
ConfigInterface *wifiConfig;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(VIBRATOR_PIN, OUTPUT);
#ifdef ESP8266
  pinMode(LED_PIN, OUTPUT);
#endif

  neopix.init();
  indic.init(&neopix);

  BootModeChecker bootChecker([&](bool ledOn) {
    if (ledOn) { indic.setHigh(); }
    else { indic.setLow(); }
    indic.update();
  });

  bootMode = bootChecker.check();

  if (bootMode == WiFiBootModeNormal) {
    router.init(&config, &indic, &neopix, &vibro, &button, &imu, &serial, &wifi);
  } else {
    wifiConfig = new ConfigInterface();
    wifiConfig->init(&config);
  }
}

void loop() {
  if (bootMode == WiFiBootModeNormal) {
    router.update();
  } else {
    wifiConfig->update();
  }

  delay(MAIN_EXECUTION_PERIOD);
}