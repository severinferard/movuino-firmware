#ifndef _MOVUINO_FIRMWARE_CONFIG_H_
#define _MOVUINO_FIRMWARE_CONFIG_H_

#include <Arduino.h>
#include "globals.h"

class Config {
private:
  // stays false if there is a problem with SPIFFS on init,
  // thus ignoring calls to load / store.
  bool disabled;

  char initialized[MAX_CONFIG_STRING_SIZE];
  char movuinoId[MAX_CONFIG_STRING_SIZE];

  bool useWiFi;

  char ssid[MAX_CONFIG_STRING_SIZE];
  char password[MAX_CONFIG_STRING_SIZE];
  char hostIP[MAX_CONFIG_STRING_SIZE];
  int portIn;
  int portOut;

  int accelRange;
  int gyroRange;

  // except for sensors, button and vibrator,
  // serial comm is never really disabled
  bool useSerial;
  bool sendSingleFrame;
  // for sensors
  int readMagPeriod;
  int outputFramePeriod;
  int buttonHoldDuration;

  // enable / disable features like heartbeat, handshake, etc here

public:
  Config() :
  disabled(true),
  useWiFi(DEFAULT_ENABLE_WIFI),
  portIn(DEFAULT_OSC_INPUT_PORT),
  portOut(DEFAULT_OSC_OUTPUT_PORT),
  accelRange(DEFAULT_ACCEL_RANGE),
  gyroRange(DEFAULT_GYRO_RANGE),
  useSerial(DEFAULT_USE_SERIAL),
  sendSingleFrame(DEFAULT_SEND_SINGLE_FRAME),
  readMagPeriod(DEFAULT_READ_MAG_PERIOD),
  outputFramePeriod(DEFAULT_OUTPUT_FRAME_PERIOD),
  buttonHoldDuration(DEFAULT_BUTTON_HOLD_DURATION) {
    strcpy(initialized, "uninitialized");
    strcpy(movuinoId, "");
    strcpy(ssid, "my_network_ssid");
    strcpy(password, "my_network_pass");
    strcpy(hostIP, "192.168.0.100");
  }

  ~Config() {}

  void init();
  void load();
  void store();

  //========================= ALL GETTERS AND SETTERS ========================//
  bool getInitialized();
  void setInitialized(bool b);

  const char *getMovuinoId();
  // void setMovuinoId(const char *id);

  bool getUseWiFi();
  void setUseWiFi(bool b);

  const char *getSsid();
  void setSsid(const char *s);

  const char *getPassword();
  void setPassword(const char *p);

  const char *getHostIP();
  void setHostIP(const char *ip);

  int getInputPort();
  void setInputPort(int p);

  int getOutputPort();
  void setOutputPort(int p);

  int getAccelRange();
  void setAccelRange(int r);

  int getGyroRange();
  void setGyroRange(int r);

  bool getUseSerial();
  void setUseSerial(bool b);

  bool getSendSingleFrame();
  void setSendSingleFrame(bool b);

  int getReadMagPeriod();
  void setReadMagPeriod(int p);

  int getOutputFramePeriod();
  void setOutputFramePeriod(int p);

  int getButtonHoldDuration();
  void setButtonHoldDuration(int d);
};

#endif /* _MOVUINO_FIRMWARE_CONFIG_H_ */
