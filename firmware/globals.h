#ifndef _MOVUINO_FIRMWARE_GLOBALS_H_
#define _MOVUINO_FIRMWARE_GLOBALS_H_

#define MAX_CONFIG_STRING_SIZE 60

#define MAX_OSC_ADDRESSES 30
#define MAX_OSC_ADDRESS_LENGTH 50
#define MAX_OSC_STRING_ARG_LENGTH 50

#define DEFAULT_ENABLE_WIFI false
#define WIFI_CONNECTION_TIMEOUT 15000

#define DEFAULT_OSC_INPUT_PORT 7400
#define DEFAULT_OSC_OUTPUT_PORT 7401

#define DEFAULT_WEB_SERVER_PORT 80 // not used ATM
#define DEFAULT_WEBSOCKET_SERVER_PORT 81 // not used ATM

/*
 * From the MPU6050 library docs :
 * 0 = +/- 2g
 * 1 = +/- 4g
 * 2 = +/- 8g
 * 3 = +/- 16g
 */
#define DEFAULT_ACCEL_RANGE 2 // +/- 8g

/*
 * From the MPU6050 library docs :
 * 0 = +/- 250 degrees/sec
 * 1 = +/- 500 degrees/sec
 * 2 = +/- 1000 degrees/sec
 * 3 = +/- 2000 degrees/sec
 */
#define DEFAULT_GYRO_RANGE 2 // +/- 1000 degrees/sec

#define DEFAULT_USE_SERIAL true // for sensors, button and vibrator
#define DEFAULT_SEND_SINGLE_FRAME true // pure UDP, no websocket

#define DEFAULT_READ_MAG_PERIOD 10
#define DEFAULT_OUTPUT_FRAME_PERIOD 10
#define DEFAULT_BUTTON_HOLD_DURATION 500

#define BUTTON_BOOT_HOLD_DURATION 1000

#define LOW_BLINK_PERIOD 200
#define FAST_BLINK_PERIOD 50

const int pinBtn = 13;     // the number of the pushbutton pin
const int pinLedWifi = 2;  // wifi led indicator
const int pinLedBat = 0;   // battery led indicator
const int pinVibro = 14;   // vibrator pin

//================================= ENUMS ====================================//

enum ButtonState {
  ButtonNone = 0,
  ButtonPressed,
  ButtonReleased,
  ButtonHolding
};

enum WiFiBootMode {
  WiFiStation = 0,
  WiFiAccessPoint
};

enum WiFiConnectionState {
  WiFiDisconnected = 0,
  WiFiConnecting,
  WiFiConnected
};

//=============================== BOOT MODE ==================================//

// return true if AP mode required by holding button from boot
// during BUTTON_BOOT_HOLD_DURATION ms

static WiFiBootMode wifiBootMode = WiFiStation;

static WiFiBootMode checkBootMode() {
  bool btnOn = digitalRead(pinBtn) == 0;
  unsigned long elapsedTime = 0;
  unsigned long bootDate = millis();

  while (btnOn && elapsedTime < BUTTON_BOOT_HOLD_DURATION) {
    delay(10);
    digitalWrite(pinLedWifi, LOW);
    btnOn = digitalRead(pinBtn) == 0;
    elapsedTime = millis() - bootDate;
  }

  // in case we release the button in the same loop where elapsedTime becomes
  // >= BUTTON_BOOT_HOLD_DURATION
  if (btnOn && elapsedTime >= BUTTON_BOOT_HOLD_DURATION) {
    bool light = true;

    // blink fast until release
    while (btnOn) {
      digitalWrite(pinLedWifi, light ? LOW : HIGH); // turn OFF wifi led
      delay(FAST_BLINK_PERIOD);
      btnOn = digitalRead(pinBtn) == 0;
      light = !light;
    }

    digitalWrite(pinLedWifi, LOW);
    return WiFiAccessPoint;
  }

  return WiFiStation;
}

//============================= OSC ADDRESSES ================================//

static char oscAddresses[MAX_OSC_ADDRESSES][MAX_OSC_ADDRESS_LENGTH];

enum oscAddress {
  // input messages
  oscInputWiFiEnable = 0,
  oscInputSetWiFi,
  oscInputGetWiFi,
  oscInputSetRange,
  oscInputGetRange,
  oscInputSetConfig,
  oscInputGetConfig,
  oscInputSetAll,
  oscInputGetAll,
  oscInputVibroPulse,
  oscInputVibroNow,
  // output messages
  oscOutputWiFiState,
  oscOutputSetWiFi,
  oscOutputGetWiFi,
  oscOutputSetRange,
  oscOutputGetRange,
  oscOutputSetConfig,
  oscOutputGetConfig,
  oscOutputSetAll,
  oscOutputGetAll,
  oscOutputLocalIP,
  oscOutputSensors,
  oscOutputButton,
  oscOutputFrame
  // oscHeartBeat,
  // oscPing,
};

static void initOSCAddress(oscAddress a, const char *movuinoId, const char *path) {
  strcpy(oscAddresses[a], "/movuino/");
  strcat(oscAddresses[a], movuinoId);
  strcat(oscAddresses[a], path);
}

static void initOSCAddresses(const char *movuinoId) {
  // input messages
  initOSCAddress(oscInputWiFiEnable, movuinoId, "/wifi/enable");
  // getters / setters
  initOSCAddress(oscInputSetWiFi, movuinoId, "/wifi/set");
  initOSCAddress(oscInputGetWiFi, movuinoId, "/wifi/get");
  initOSCAddress(oscInputSetRange, movuinoId, "/range/set");
  initOSCAddress(oscInputGetRange, movuinoId, "/range/get");
  initOSCAddress(oscInputSetConfig, movuinoId, "/config/set"); // useSerial, useWiFi, readMagPeriod, outputFramePeriod
  initOSCAddress(oscInputGetConfig, movuinoId, "/config/get"); // useSerial, useWiFi
  initOSCAddress(oscInputSetAll, movuinoId, "/all/set");
  initOSCAddress(oscInputGetAll, movuinoId, "/all/get");
  // control
  initOSCAddress(oscInputVibroPulse, movuinoId, "/vibroPulse");
  initOSCAddress(oscInputVibroNow, movuinoId, "/vibroNow");

  // output messages
  initOSCAddress(oscOutputWiFiState, movuinoId, "/wifi/state"); // serial, trigger (sysex) => wifi state : 0 (disconnected), 1 (connected), 2 (connecting)
  // acks for getters / setters
  initOSCAddress(oscOutputSetWiFi, movuinoId, "/wifi/set");
  initOSCAddress(oscOutputGetWiFi, movuinoId, "/wifi/get");
  initOSCAddress(oscOutputSetRange, movuinoId, "/range/set");
  initOSCAddress(oscOutputGetRange, movuinoId, "/range/get");
  initOSCAddress(oscOutputSetConfig, movuinoId, "/config/set");
  initOSCAddress(oscOutputGetConfig, movuinoId, "/config/get");
  initOSCAddress(oscOutputSetAll, movuinoId, "/all/set");
  initOSCAddress(oscOutputGetAll, movuinoId, "/all/get");
  // actual data
  initOSCAddress(oscOutputLocalIP, movuinoId, "/localIP"); // all, trigger (sysex) => String
  initOSCAddress(oscOutputSensors, movuinoId, "/sensors"); // all, flow => ax, ay, az, gx, gy, gz, mx, my, mz, btn
  initOSCAddress(oscOutputButton, movuinoId, "/button"); // all, trigger (sysex) => 0 (off), 1 (on), 2 (hold)
  initOSCAddress(oscOutputFrame, movuinoId, "/frame"); // contains sensors, button, vibrating, ip (12 values)
}

#endif /* _MOVUINO_FIRMWARE_GLOBALS_H_ */
