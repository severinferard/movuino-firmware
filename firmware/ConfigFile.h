#ifndef _MOVUINO_CONFIG_FILE_H_
#define _MOVUINO_CONFIG_FILE_H_

// #ifdef ESP8266
// #include <ESP8266WiFi.h>
// #elif defined(ESP32)
// #include <WiFi.h>
// #endif

#include <ESPConfigFile.h>
#include "WiFiInterface.h"
#include "globals.h"

class ConfigFile {
private:
  char firmwareVersion[12]; // xxx.xxx.xxx + '\0'
  char movuinoUUID[13]; // 12 mac characters + '\0'
  char apSsid[21]; // "movuino-" + 12 mac characters + '\0'
  ESPConfigFile *config;

public:
  ConfigFile() {
    config = new ESPConfigFile("config"); // will look for "/config.txt" file
  }

  ~ConfigFile() {
    delete config;
  }

  void init(WiFiInterface *wifi) {
    sprintf(firmwareVersion, "v%i.%i.%i",
      MOVUINO_FIRMWARE_VERSION_MAJOR,
      MOVUINO_FIRMWARE_VERSION_MINOR,
      MOVUINO_FIRMWARE_VERSION_PATCH
    );

    strcpy(movuinoUUID, wifi->getMovuinoUUID());

    strcpy(apSsid, "movuino-");
    strcat(apSsid, wifi->getMovuinoUUID());

    config->addBoolParameter("useSerial", DEFAULT_USE_SERIAL);
    config->addBoolParameter("useWiFi", DEFAULT_USE_WIFI);
    config->addBoolParameter("readMag", DEFAULT_READ_MAG);
    config->addBoolParameter("useNeoPixelAsIndicator", DEFAULT_USE_NEOPIXEL_AS_INDICATOR);

    config->addIntParameter("oscOutputPeriod", DEFAULT_OSC_OUTPUT_PERIOD);

    config->addIntParameter("udpInputPort", DEFAULT_UDP_INPUT_PORT);
    config->addIntParameter("udpOutputPort", DEFAULT_UDP_OUTPUT_PORT);

    config->addIntParameter("accelRange", DEFAULT_ACCEL_RANGE);
    config->addIntParameter("gyroRange", DEFAULT_GYRO_RANGE);

    config->addStringParameter("movuinoID", DEFAULT_MOVUINO_ID);

    config->addBoolParameter("stationMode", DEFAULT_STATION_MODE);
    config->addBoolParameter("dhcpOn", DEFAULT_DHCP_ON);

    config->addStringParameter("apSsid", static_cast<const char *>(apSsid));
    config->addStringParameter("apPass", DEFAULT_AP_PASS);

    config->addStringParameter("staSsid", DEFAULT_STA_SSID);
    config->addStringParameter("staPass", DEFAULT_STA_PASS);

    config->addIntParameter("hostIP1", DEFAULT_HOST_IP_1);
    config->addIntParameter("hostIP2", DEFAULT_HOST_IP_2);
    config->addIntParameter("hostIP3", DEFAULT_HOST_IP_3);
    config->addIntParameter("hostIP4", DEFAULT_HOST_IP_4);

    config->addIntParameter("staticIP1", DEFAULT_STATIC_IP_1);
    config->addIntParameter("staticIP2", DEFAULT_STATIC_IP_2);
    config->addIntParameter("staticIP3", DEFAULT_STATIC_IP_3);
    config->addIntParameter("staticIP4", DEFAULT_STATIC_IP_4);

    config->addIntParameter("gatewayIP1", DEFAULT_GATEWAY_IP_1);
    config->addIntParameter("gatewayIP2", DEFAULT_GATEWAY_IP_2);
    config->addIntParameter("gatewayIP3", DEFAULT_GATEWAY_IP_3);
    config->addIntParameter("gatewayIP4", DEFAULT_GATEWAY_IP_4);

    config->addIntParameter("subnetIP1", DEFAULT_SUBNET_IP_1);
    config->addIntParameter("subnetIP2", DEFAULT_SUBNET_IP_2);
    config->addIntParameter("subnetIP3", DEFAULT_SUBNET_IP_3);
    config->addIntParameter("subnetIP4", DEFAULT_SUBNET_IP_4);

    config->load();

    config->setIntParameter("staticIP1", DEFAULT_STATIC_IP_1);
    config->setIntParameter("staticIP2", DEFAULT_STATIC_IP_2);
    config->setIntParameter("staticIP3", DEFAULT_STATIC_IP_3);
    config->setIntParameter("staticIP4", DEFAULT_STATIC_IP_4);

    config->setIntParameter("gatewayIP1", DEFAULT_GATEWAY_IP_1);
    config->setIntParameter("gatewayIP2", DEFAULT_GATEWAY_IP_2);
    config->setIntParameter("gatewayIP3", DEFAULT_GATEWAY_IP_3);
    config->setIntParameter("gatewayIP4", DEFAULT_GATEWAY_IP_4);

    config->setIntParameter("subnetIP1", DEFAULT_SUBNET_IP_1);
    config->setIntParameter("subnetIP2", DEFAULT_SUBNET_IP_2);
    config->setIntParameter("subnetIP3", DEFAULT_SUBNET_IP_3);
    config->setIntParameter("subnetIP4", DEFAULT_SUBNET_IP_4);

    config->store();
  }

  void load() { config->load(); }
  void store() { config->store(); }
  void clear() { config->clear(); }

  ////////// getters :

  bool getUseSerial() { return config->getBoolParameter("useSerial"); }
  bool getUseWiFi() { return config->getBoolParameter("useWiFi"); }
  bool getReadMag() { return config->getBoolParameter("readMag"); }
  bool getUseNeoPixelAsIndicator() { return config->getBoolParameter("useNeoPixelAsIndicator"); }

  int getOSCOutputPeriod() { return static_cast<int>(config->getIntParameter("oscOutputPeriod")); }

  int getUdpInputPort() { return static_cast<int>(config->getIntParameter("udpInputPort")); }
  int getUdpOutputPort() { return static_cast<int>(config->getIntParameter("udpOutputPort")); }

  int getAccelRange() { return static_cast<int>(config->getIntParameter("accelRange")); }
  int getGyroRange() { return static_cast<int>(config->getIntParameter("gyroRange")); }

  const char *getMovuinoID() { return config->getStringParameter("movuinoID"); }

  bool getStationMode() { return config->getBoolParameter("stationMode"); }
  bool getDHCPOn() { return config->getBoolParameter("dhcpOn"); }

  const char *getApSsid() { return config->getStringParameter("apSsid"); }
  const char *getApPass() { return config->getStringParameter("apPass"); }

  const char *getStaSsid() { return config->getStringParameter("staSsid"); }
  const char *getStaPass() { return config->getStringParameter("staPass"); }

  IPAddress getHostIP(/*IPAddress& ip*/) {
    return IPAddress(
      config->getIntParameter("hostIP1"),
      config->getIntParameter("hostIP2"),
      config->getIntParameter("hostIP3"),
      config->getIntParameter("hostIP4")
    );
    // ip[0] = config->getIntParameter("hostIP1");
    // ip[1] = config->getIntParameter("hostIP2");
    // ip[2] = config->getIntParameter("hostIP3");
    // ip[3] = config->getIntParameter("hostIP4");
  }

  IPAddress getStaticIP(/*IPAddress& ip*/) {
    return IPAddress(
      config->getIntParameter("staticIP1"),
      config->getIntParameter("staticIP2"),
      config->getIntParameter("staticIP3"),
      config->getIntParameter("staticIP4")
    );
    // ip[0] = config->getIntParameter("staticIP1");
    // ip[1] = config->getIntParameter("staticIP2");
    // ip[2] = config->getIntParameter("staticIP3");
    // ip[3] = config->getIntParameter("staticIP4");
  }

  IPAddress getGatewayIP(/*IPAddress& ip*/) {
    return IPAddress(
      config->getIntParameter("gatewayIP1"),
      config->getIntParameter("gatewayIP2"),
      config->getIntParameter("gatewayIP3"),
      config->getIntParameter("gatewayIP4")
    );
    // ip[0] = config->getIntParameter("gatewayIP1");
    // ip[1] = config->getIntParameter("gatewayIP2");
    // ip[2] = config->getIntParameter("gatewayIP3");
    // ip[3] = config->getIntParameter("gatewayIP4");
  }

  IPAddress getSubnetIP(/*IPAddress& ip*/) {
    return IPAddress(
      config->getIntParameter("subnetIP1"),
      config->getIntParameter("subnetIP2"),
      config->getIntParameter("subnetIP3"),
      config->getIntParameter("subnetIP4")
    );
    // ip[0] = config->getIntParameter("subnetIP1");
    // ip[1] = config->getIntParameter("subnetIP2");
    // ip[2] = config->getIntParameter("subnetIP3");
    // ip[3] = config->getIntParameter("subnetIP4");
  }

  const char *getFirmwareVersion() { return static_cast<const char *>(firmwareVersion); }
  const char *getMovuinoUUID() { return static_cast<const char *>(movuinoUUID); }

  ////////// setters :

  void setUseSerial(bool u) { config->setBoolParameter("useSerial", u); }
  void setUseWiFi(bool u) { config->setBoolParameter("useWiFi", u); }
  void setReadMag(bool r) { config->setBoolParameter("readMag", r); }
  void setUseNeoPixelAsIndicator(bool u) { config->setBoolParameter("useNeoPixelAsIndicator", u); }

  void setOSCOutputPeriod(int p) { config->setIntParameter("oscOutputPeriod", p); }

  void setUdpInputPort(int p) { config->setIntParameter("udpInputPort", p); }
  void setUdpOutputPort(int p) { config->setIntParameter("udpOutputPort", p); }

  void setAccelRange(int r) { config->setIntParameter("accelRange", r); }
  void setGyroRange(int r) { config->setIntParameter("gyroRange", r); }

  void setMovuinoID(const char *id) { config->setStringParameter("movuinoID", id); }

  void setStationMode(bool s) { config->setBoolParameter("stationMode", s); }
  void setDHCPOn(bool d) { config->setBoolParameter("dhcpOn", d); }

  void setApSsid(const char *ssid) { config->setStringParameter("apSsid", ssid); }
  void setApPass(const char *pass) { config->setStringParameter("apPass", pass); }

  void setStaSsid(const char *ssid) { config->setStringParameter("staSsid", ssid); }
  void setStaPass(const char *pass) { config->setStringParameter("staPass", pass); }

  void setHostIP(IPAddress& ip) {
    config->setIntParameter("hostIP1", ip[0]);
    config->setIntParameter("hostIP2", ip[1]);
    config->setIntParameter("hostIP3", ip[2]);
    config->setIntParameter("hostIP4", ip[3]);
  }

  void setStaticIP(IPAddress& ip) {
    config->setIntParameter("staticIP1", ip[0]);
    config->setIntParameter("staticIP2", ip[1]);
    config->setIntParameter("staticIP3", ip[2]);
    config->setIntParameter("staticIP4", ip[3]);
  }

  void setGatewayIP(IPAddress& ip) {
    config->setIntParameter("gatewayIP1", ip[0]);
    config->setIntParameter("gatewayIP2", ip[1]);
    config->setIntParameter("gatewayIP3", ip[2]);
    config->setIntParameter("gatewayIP4", ip[3]);
  }

  void setSubnetIP(IPAddress& ip) {
    config->setIntParameter("subnetIP1", ip[0]);
    config->setIntParameter("subnetIP2", ip[1]);
    config->setIntParameter("subnetIP3", ip[2]);
    config->setIntParameter("subnetIP4", ip[3]);
  }
};

#endif /* _MOVUINO_CONFIG_FILE_H_ */