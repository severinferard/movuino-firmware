#ifndef _MOVUINO_CONFIG_INTERFACE_H_
#define _MOVUINO_CONFIG_INTERFACE_H_

#include <ESPWiFiInterfaceWebServer.h>
#include "ConfigFile.h"
#include "configWebPage.h"

class ConfigInterface {
private:
  ConfigFile *config;
  ESPWiFiInterfaceWebServer *server;

  int msgLength;
  String msg[MAX_CONFIG_ITEMS];

public:
  ConfigInterface(ConfigFile *c) {
    config = c;

    server = new ESPWiFiInterfaceWebServer();

    server->enableDHCP(false);
    server->setStaticIP(config->getStaticIP());
    server->setRootWebPage(configWebPage);

    server->setSocketConnectionListener([&](uint8_t num) {
      sendConfiguration(num);
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

  void update() {
    server->update();
  }

private:
  void onSocketMessage(uint8_t num, const char *message, size_t length) {

    ////////// PARSE INPUT SOCKET MESSAGE

    msgLength = 0;
    msg[0] = String("");

    for (size_t index = 0; index < length; index++) {
      if (message[index] != '\n') {
        msg[msgLength] += message[index];
      } else if (index < length - 1) {
        msgLength++;
        if (msgLength == MAX_CONFIG_ITEMS) break;
        msg[msgLength] = String("");
      }
    }

    msgLength++;

    ////////// PROCESS PARSED MESSAGE

    if (msgLength > 0) {
      if (msg[0] == "clear") {
        config->clear();
        config->store();
        sendConfiguration(num); // refresh page
      } else if (msg[0] == "settings" && msgLength >= 13) {
        config->setMovuinoID(msg[1].c_str());
        config->setUseWiFi(msg[2].toInt() > 0);
        config->setStationMode(msg[3].toInt() == 0);
        config->setStaSsid(msg[4].c_str());
        config->setStaPass(msg[5].c_str());

        IPAddress ip(msg[6].toInt(), msg[7].toInt(), msg[8].toInt(), msg[9].toInt());
        config->setHostIP(ip);

        config->setUdpInputPort(msg[10].toInt());
        config->setUdpOutputPort(msg[11].toInt());
        config->setAccelRange(msg[12].toInt());
        config->setGyroRange(msg[13].toInt());
        config->setUseSerial(msg[14].toInt() > 0);
        config->setReadMag(msg[15].toInt() > 0);
        config->setOSCOutputPeriod(msg[16].toInt());
        config->store();
        sendConfiguration(num);
      } else {
        // ... something else needed ?
      }
    }
  }

  void sendConfiguration(uint8_t num) {
    /*
    String settings("settings\n");

    settings += config->getMovuinoID();// + "\n";
    settings += "\n";
    settings += config->getUseWiFi() ? "1\n" : "0\n";
    settings += config->getStationMode() ? "0\n" : "1\n";
    settings += config->getStaSsid();// + "\n";
    settings += "\n";
    settings += config->getStaPass();// + "\n";
    settings += "\n";

    IPAddress ip = config->getHostIP();
    settings += static_cast<unsigned int>(ip[0]) + "\n";
    settings += static_cast<unsigned int>(ip[1]) + "\n";
    settings += static_cast<unsigned int>(ip[2]) + "\n";
    settings += static_cast<unsigned int>(ip[3]) + "\n";

    settings += config->getUdpInputPort() + "\n";
    settings += config->getUdpOutputPort() + "\n";

    settings += config->getAccelRange() + "\n";
    settings += config->getGyroRange() + "\n";

    settings += config->getUseSerial() ? "1\n" : "0\n";
    settings += config->getReadMag() ? "1\n" : "0\n";

    settings += config->getOSCOutputPeriod() + "\n";

    settings += config->getFirmwareVersion();// + "\n";
    settings += "\n";
    settings += config->getMovuinoUUID();// + "\n";
    settings += "\n";

    server->sendTextMessageToSocket(num, settings.c_str());
    //*/

    char packet[MAX_TOTAL_CONFIG_STRING_SIZE];
    String intStr;

    strcpy(packet, "settings\n");

    strcat(packet, config->getMovuinoID());
    strcat(packet, "\n");

    strcat(packet, config->getUseWiFi() ? "1\n" : "0\n");
    strcat(packet, config->getStationMode() ? "0\n" : "1\n");

    strcat(packet, config->getStaSsid());
    strcat(packet, "\n");
    strcat(packet, config->getStaPass());
    strcat(packet, "\n");

    IPAddress ip = config->getHostIP();
    intStr = String(static_cast<unsigned int>(ip[0]));
    strcat(packet, intStr.c_str());
    strcat(packet, "\n");
    intStr = String(static_cast<unsigned int>(ip[1]));
    strcat(packet, intStr.c_str());
    strcat(packet, "\n");
    intStr = String(static_cast<unsigned int>(ip[2]));
    strcat(packet, intStr.c_str());
    strcat(packet, "\n");
    intStr = String(static_cast<unsigned int>(ip[3]));
    strcat(packet, intStr.c_str());
    strcat(packet, "\n");

    intStr = String(config->getUdpInputPort());
    strcat(packet, intStr.c_str());
    strcat(packet, "\n");
    intStr = String(config->getUdpOutputPort());
    strcat(packet, intStr.c_str());
    strcat(packet, "\n");

    intStr = String(config->getAccelRange());
    strcat(packet, intStr.c_str());
    strcat(packet, "\n");
    intStr = String(config->getGyroRange());
    strcat(packet, intStr.c_str());
    strcat(packet, "\n");

    strcat(packet, config->getUseSerial() ? "1\n" : "0\n");    
    strcat(packet, config->getReadMag() ? "1\n" : "0\n");

    intStr = String(config->getOSCOutputPeriod());
    strcat(packet, intStr.c_str());
    strcat(packet, "\n");

    strcat(packet, config->getFirmwareVersion());
    strcat(packet, "\n");
    strcat(packet, config->getMovuinoUUID());
    strcat(packet, "\n");

    server->sendTextMessageToSocket(num, static_cast<const char *>(packet));
  }
};

#endif /* _MOVUINO_CONFIG_INTERFACE_H_ */