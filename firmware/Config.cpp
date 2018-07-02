#include <FS.h>
#include <ESP8266WiFi.h>
#include "Config.h"

void
Config::init() {
  if (SPIFFS.begin()) {
    disabled = false;
  }

  byte mac[6];
  WiFi.macAddress(mac);

  String _movuinoId(mac[0], HEX);
  for (int i = 1; i < 6; i++) {
    // res += ":";
    _movuinoId += String(mac[i], HEX);
  }

  // _movuinoId = "movuino-" + _movuinoId;
  _movuinoId.toCharArray(movuinoId, _movuinoId.length());

}

void
Config::load() {
  if (disabled) return;

  File file = SPIFFS.open("/config.txt", "r");
  if (file) {
    String _initialized = file.readStringUntil('\n');

    char tmpInitialized[MAX_CONFIG_STRING_SIZE];
    _initialized.toCharArray(tmpInitialized, _initialized.length());

    if (strcmp(tmpInitialized, "initialized") != 0) return;

    useWiFi = file.readStringUntil('\n').toInt() > 0;

    String _ssid = file.readStringUntil('\n');
    String _password = file.readStringUntil('\n');
    String _hostIP = file.readStringUntil('\n');

    _initialized.toCharArray(initialized, _initialized.length());
    _ssid.toCharArray(ssid, _ssid.length());
    _password.toCharArray(password, _password.length());
    _hostIP.toCharArray(hostIP, _hostIP.length());

    portIn = file.readStringUntil('\n').toInt();
    portOut = file.readStringUntil('\n').toInt();

    accelRange = file.readStringUntil('\n').toInt();
    gyroRange = file.readStringUntil('\n').toInt();

    useSerial = file.readStringUntil('\n').toInt() > 0;
    sendSingleFrame = file.readStringUntil('\n').toInt() > 0;

    readMagPeriod = file.readStringUntil('\n').toInt();
    outputFramePeriod = file.readStringUntil('\n').toInt();
    buttonHoldDuration = file.readStringUntil('\n').toInt();

    file.close();
  }
}

void
Config::store() {
  if (disabled) return;

  File file = SPIFFS.open("/config.txt", "w+");
  if (file) {
    if (strcmp(initialized, "initialized") != 0){
      strcpy(initialized, "initialized");
    }

    file.println(initialized);

    file.println(useWiFi ? "1" : "0");

    file.println(ssid);
    file.println(password);
    file.println(hostIP);
    file.println(portIn);
    file.println(portOut);

    file.println(accelRange);
    file.println(gyroRange);

    file.println(useSerial ? "1" : "0");
    file.println(sendSingleFrame ? "1" : "0");
    file.println(readMagPeriod);
    file.println(outputFramePeriod);
    file.println(buttonHoldDuration);

    file.close();
  }
}

//========================= GETTERS AND SETTERS ==============================//

const char *
Config::getMovuinoId() {
  return movuinoId;
}

bool
Config::getInitialized() {
  return strcmp(initialized, "initialized") == 0;
}

void
Config::setInitialized(bool b) {
  strcpy(initialized, b ? "initialized" : "uninitialized");
}

////////// WIFI SETTINGS

bool
Config::getUseWiFi() {
  return useWiFi;
}

void
Config::setUseWiFi(bool b) {
  useWiFi = b;
}

const char *
Config::getSsid() {
  return ssid;
}

void
Config::setSsid(const char *s) {
  strcpy(ssid, s);
}

const char *
Config::getPassword() {
  return password;
}

void
Config::setPassword(const char *p) {
  strcpy(password, p);
}

const char *
Config::getHostIP() {
  return hostIP;
}

void
Config::setHostIP(const char *ip) {
  strcpy(hostIP, ip);
}

////////// INPUT / OUTPUT OSC PORTS

int
Config::getInputPort() {
  return portIn;
}

void
Config::setInputPort(int p) {
  portIn = p;
}

int
Config::getOutputPort() {
  return portOut;
}

void
Config::setOutputPort(int p) {
  portOut = p;
}

////////// ACCEL / GYRO SENSITIVITY :

/*
 * From the MPU6050 library's doc :
 * 0 = +/- 2g
 * 1 = +/- 4g
 * 2 = +/- 8g
 * 3 = +/- 16g
 */

int
Config::getAccelRange() {
  return accelRange;
}

void
Config::setAccelRange(int r) {
  accelRange = r < 0 ? 0 : (r > 3 ? 3 : r);
}

/*
 * From the MPU6050 library's doc :
 * 0 = +/- 250 degrees/sec
 * 1 = +/- 500 degrees/sec
 * 2 = +/- 1000 degrees/sec
 * 3 = +/- 2000 degrees/sec
 */

int
Config::getGyroRange() {
  return gyroRange;
}

void
Config::setGyroRange(int r) {
  gyroRange = r < 0 ? 0 : (r > 3 ? 3 : r);
}

////////// OTHER CONFIG PARAMETERS

bool
Config::getUseSerial() {
  return useSerial;
}

void
Config::setUseSerial(bool b) {
  useSerial = b;
}

bool
Config::getSendSingleFrame() {
  return sendSingleFrame;
}

void
Config::setSendSingleFrame(bool b) {
  sendSingleFrame = b;
}

int
Config::getReadMagPeriod() {
  return readMagPeriod;
}

void
Config::setReadMagPeriod(int p) {
  readMagPeriod = p;
}

int
Config::getOutputFramePeriod() {
  return outputFramePeriod;
}

void
Config::setOutputFramePeriod(int p) {
  outputFramePeriod = p;
}

int
Config::getButtonHoldDuration() {
  return buttonHoldDuration;
}

void
Config::setButtonHoldDuration(int p) {
  buttonHoldDuration = p;
}
