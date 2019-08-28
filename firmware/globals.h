#ifndef _MOVUINO_GLOBALS_H_
#define _MOVUINO_GLOBALS_H_

#include <Arduino.h>

////////// FIRMWARE VERSION

#define MOVUINO_FIRMWARE_VERSION_MAJOR 2
#define MOVUINO_FIRMWARE_VERSION_MINOR 0
#define MOVUINO_FIRMWARE_VERSION_PATCH 0

////////// PIN NUMBERS

#define BUTTON_PIN 13
#define LED_PIN 2
#define VIBRATOR_PIN 14
#define NEOPIXEL_PIN 15

#ifdef ESP32
#define SDA_PIN 21
#define SCL_PIN 22
#endif

////////// DEFAULT CONFIGURATION VALUES

#define DEFAULT_USE_WIFI true
#define DEFAULT_USE_SERIAL true
#define DEFAULT_READ_MAG true

#ifdef ESP8266
#define DEFAULT_USE_NEOPIXEL_AS_INDICATOR false
#elif defined(ESP32)
#define DEFAULT_USE_NEOPIXEL_AS_INDICATOR true
#endif

#define DEFAULT_OSC_OUTPUT_PERIOD 10

#define DEFAULT_UDP_INPUT_PORT 7401
#define DEFAULT_UDP_OUTPUT_PORT 7400

#define DEFAULT_ACCEL_RANGE 3 // +/- 16 G
#define DEFAULT_GYRO_RANGE 3 // +/- 2000 DPS

#define DEFAULT_MOVUINO_ID "movuino"

#define DEFAULT_STATION_MODE true
#define DEFAULT_DHCP_ON true

#define DEFAULT_AP_SSID "movuino"
#define DEFAULT_AP_PASS ""

#define DEFAULT_STA_SSID "my_network_ssid"
#define DEFAULT_STA_PASS "my_network_pass"

#define DEFAULT_HOST_IP_1 192
#define DEFAULT_HOST_IP_2 168
#define DEFAULT_HOST_IP_3 0
#define DEFAULT_HOST_IP_4 100

#define DEFAULT_STATIC_IP_1 192
#define DEFAULT_STATIC_IP_2 168
#define DEFAULT_STATIC_IP_3 1
#define DEFAULT_STATIC_IP_4 1

#define DEFAULT_GATEWAY_IP_1 192
#define DEFAULT_GATEWAY_IP_2 168
#define DEFAULT_GATEWAY_IP_3 1
#define DEFAULT_GATEWAY_IP_4 1

#define DEFAULT_SUBNET_IP_1 255
#define DEFAULT_SUBNET_IP_2 255
#define DEFAULT_SUBNET_IP_3 255
#define DEFAULT_SUBNET_IP_4 0

////////// NON CONFIGURABLE BEHAVIOUR VALUES

#define MAIN_EXECUTION_PERIOD 1
#define MAX_OSC_STRING_ARG_LENGTH 64
#define MAX_CONFIG_ITEMS 32
#define MAX_TOTAL_CONFIG_STRING_SIZE 256

#define BUTTON_BOOT_HOLD_DURATION 1000
#define WIFI_CONNECTION_TIMEOUT 20000

#define LOW_BLINK_PERIOD 200
#define FAST_BLINK_PERIOD 50

#define BUTTON_STATE_HOLD_DURATION 500

////////// ENUMS

enum ButtonState {
  ButtonStateNone = 0,
  ButtonStatePressed,
  ButtonStateReleased,
  ButtonStateHolding
};

enum WiFiBootMode {
  WiFiBootModeNormal = 0,
  WiFiBootModeConfig
};

////////// UTILITIES

class BootModeChecker {
private:
  void (*callback)(bool ledOn);

public:
  BootModeChecker(void (*cb)(bool ledOn)) : callback(cb) {}
  ~BootModeChecker() {}

  WiFiBootMode check() {
    bool btnOn = digitalRead(BUTTON_PIN) == LOW;
    unsigned long elapsedTime = 0;
    unsigned long bootDate = millis();

    while (btnOn && elapsedTime < BUTTON_BOOT_HOLD_DURATION) {
      delay(10);
      callback(true);
      btnOn = digitalRead(BUTTON_PIN) == LOW;
      elapsedTime = millis() - bootDate;
    }

    if (btnOn && elapsedTime >= BUTTON_BOOT_HOLD_DURATION) {
      bool light = true;

      while (btnOn) {
        callback(light);
        delay(FAST_BLINK_PERIOD);
        btnOn = digitalRead(BUTTON_PIN) == LOW;
        light = !light;
      }

      callback(true);
      return WiFiBootModeConfig;
    }

    return WiFiBootModeNormal;
  }
};

#endif /* _MOVUINO_GLOBALS_H_ */