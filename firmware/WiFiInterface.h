#ifndef _MOVUINO_WIFI_INTERFACE_H_
#define _MOVUINO_WIFI_INTERFACE_H_

#include <ESPWiFiInterfaceBase.h>
#include <WiFiUdp.h>
#include "OSCInterface.h"

class WiFiInterface : public ESPWiFiInterfaceBase, public OSCInterface {
private:
  WiFiUDP udp;
  IPAddress hostIP;  
  int udpInputPort;
  int udpOutputPort;

  char uuid[13];

  OSCMessage inputOSCMessage;

public:
  WiFiInterface() :
  ESPWiFiInterfaceBase(),
  OSCInterface(),
  udpInputPort(8001),
  udpOutputPort(8000) {
    hostIP = IPAddress(0, 0, 0, 0);

    uint8_t mac[6];
    getMacAddress(&(mac[0]));
    sprintf(
      uuid, "%02X%02X%02X%02X%02X%02X",
      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
    );
  }

  ~WiFiInterface() {}

  void startWiFi(WiFiMode_t mode, const char *ssid, const char *pass = "") {
    if (isActive()) stopWiFi();
    
    ESPWiFiInterfaceBase::startWiFi(mode, ssid, pass);
    udp.begin(udpInputPort);
  }

  void stopWiFi() {
    ESPWiFiInterfaceBase::stopWiFi();
    udp.stop();
  }

  void update() {
    ESPWiFiInterfaceBase::update();

    if (isActive()) {
      int packetSize = udp.parsePacket();
      inputOSCMessage.empty();

      if (packetSize > 0) {
        while (packetSize--) {
          inputOSCMessage.fill(udp.read());
        }

        if (!inputOSCMessage.hasError()) {
          oscEmitter.emitOSCMessage(inputOSCMessage);
        }
      }
    }
  }

  void setHostIP(IPAddress ip) {
    hostIP = ip;
  }
  
  void setUdpInputPort(int port) {
    if (isActive() && udpInputPort != port) {
      udp.begin(port);
    }

    udpInputPort = port;
  }

  void setUdpOutputPort(int port) {
    udpOutputPort = port;
  }

  void sendOSCMessage(OSCMessage& msg) {
    if (isActive()) {
      udp.beginPacket(hostIP, udpOutputPort);
      msg.send(udp);
      udp.endPacket();
    }
  }

  const char *getMovuinoUUID() {
    return static_cast<const char *>(uuid);
  }
};

#endif /* _MOVUINO_WIFI_INTERFACE_H_ */