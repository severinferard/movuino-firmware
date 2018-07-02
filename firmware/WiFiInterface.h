#ifndef _MOVUINO_FIRMWARE_WIFIINTERFACE_H_
#define _MOVUINO_FIRMWARE_WIFIINTERFACE_H_

#define MAX_OSC_ADDRESS_SIZE 120
#define MAX_OSC_ADDRESSES 100

#include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "Timer.h"
#include "globals.h"

class Config;
class Router;

//----------------------------------------------------------------------------//

class WiFiInterface : public Timer {
private:
  WiFiBootMode wifiBootMode;

  // timer specific vars
  bool wifiLight;
  bool batLight;
  unsigned long connectionTimeout;

  String wifi;
  String disconnected;
  String connecting;
  String connected;

  bool initialized;
  WiFiUDP udp;

  Config *config;
  Router *router;


public:
  WiFiInterface(unsigned long blinkPeriod = LOW_BLINK_PERIOD,
                unsigned long timeout = WIFI_CONNECTION_TIMEOUT) :
  Timer(blinkPeriod),
  wifiBootMode(WiFiStation),
  wifiLight(false),
  batLight(false),
  connectionTimeout(timeout),
  wifi("wifi"),
  disconnected("disconnected"),
  connecting("connecting"),
  connected("connected"),
  initialized(false) {}

  virtual ~WiFiInterface() {}

  // override parent methods :
  void stop(); // called back by update when timeout is over
  void callback();

  void setBootMode(WiFiBootMode m); // supposed to be called once at start time
  void init(Config *c, Router *r); // supposed to be called once at start time
  void update();

  void readMessages(/*Router *router*/);
  bool sendMessage(OSCMessage &msg, const char *hostIP, int portOut);

  String getStringMacAddress();
  void getIPAddress(int *res); // res must be of type int[4]
  String getStringIPAddress();

  bool isConnected();
  void startWiFi();
  void stopWiFi();
  void toggleWiFiState();

private:
  void onConnectionEvent(WiFiConnectionState s);
};

#endif /* _MOVUINO_FIRMWARE_WIFIINTERFACE_H_ */
