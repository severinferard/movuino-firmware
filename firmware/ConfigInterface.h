#ifndef _MOVUINO_CONFIG_INTERFACE_H_
#define _MOVUINO_CONFIG_INTERFACE_H_

#include <ESPWiFiInterfaceWebServer.h>
#include "ConfigFile.h"
#include "configWebPage.h"

class ConfigInterface {
private:
  ConfigFile *config;
  ESPWiFiInterfaceWebServer *server;

public:
  ConfigInterface() {
    server = new ESPWiFiInterfaceWebServer();

    server->enableDHCP(false);
    server->setStaticIP(IPAddress(192, 168, 1, 1));
    server->setRootWebPage(configWebPage);

    server->setSocketConnectionListener([&](uint8_t num) {
      onSocketConnection(num);
    });

    server->setSocketTextMessageListener([&](uint8_t num, const char *message,
                                             size_t length) {
      onSocketMessage(num, message, length);
    });

    uint8_t mac[6];
    char dst[21]; // "movuino-" + 12 mac characters + '\0'
    server->getMacAddress(&(mac[0]));
    sprintf(
      dst, "movuino-%02X%02X%02X%02X%02X%02X",
      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
    );

    server->startWiFi(WIFI_AP, static_cast<const char *>(dst));
  }

  ~ConfigInterface() {
    delete server;
  }

  void init(ConfigFile *c) {
    config = c;
  }

private:
  void onSocketMessage(uint8_t num, const char *message, size_t length) {
    // parse message and update / clear config
  }

  void onSocketConnection(uint8_t num) {
    
  }

  void sendConfiguration(uint8_t num) {

  }
};

#endif /* _MOVUINO_CONFIG_INTERFACE_H_ */