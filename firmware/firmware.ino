#include "Wire.h"

#include "Router.h"
#include "Config.h"
#include "Button.h"
#include "Vibrator.h"
#include "Sensors.h"
#include "SerialInterface.h"
#include "WiFiInterface.h"

#include "globals.h"

Router router;
Config config;
Button button;
Sensors sensors;
Vibrator vibrator;
SerialInterface serial;
WiFiInterface wifi;

void setup() {
  pinMode(pinBtn, INPUT_PULLUP); // pin for the button
  pinMode(pinLedWifi, OUTPUT);   // pin for the wifi led
  pinMode(pinLedBat, OUTPUT);    // pin for the battery led
  pinMode(pinVibro, OUTPUT);     // pin for the vibrator

  wifi.setBootMode(checkBootMode());

  Wire.begin();
  router.init(&config, &button, &sensors, &vibrator, &serial, &wifi);
}

void loop() {
  router.update();
}
