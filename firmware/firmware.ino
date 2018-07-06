#include "Wire.h"

#include "Router.h"
#include "Config.h"
#include "Button.h"
#include "Vibrator.h"
#include "Sensors.h"
#include "SerialInterface.h"
#include "WiFiInterface.h"
#include "AccessPoint.h"

#include "globals.h"

Router router;
Config config;
Button button;
Sensors sensors;
Vibrator vibrator;
SerialInterface serial;
WiFiInterface wifi;

AccessPoint *ap;

WiFiBootMode mode;

void setup() {
  pinMode(pinBtn, INPUT_PULLUP); // pin for the button
  pinMode(pinLedWifi, OUTPUT); // pin for the wifi led
  pinMode(pinLedBat, OUTPUT); // pin for the battery led
  pinMode(pinVibro, OUTPUT); // pin for the vibrator

  mode = checkBootMode();

  if (mode == WiFiStation) {
    Wire.begin();
    router.init(&config, &button, &sensors, &vibrator, &serial, &wifi);
  } else {
    ap = new AccessPoint();
    ap->init(&config);
  }
}

void loop() {
  if (mode == WiFiStation) {
    router.update();
  } else {
    ap->update();
  }
}
