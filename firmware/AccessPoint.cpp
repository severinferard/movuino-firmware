#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
// Don't use, mDNS not supported by Android. That's an issue.
// see : https://tttapa.github.io/ESP8266/Chap08%20-%20mDNS.html
// #include <ESP8266mDNS.h>

#include "Config.h"
#include "AccessPoint.h"
#include "configWebPage.h"

#include "globals.h" // use MAX_CONFIG_STRING_SIZE to format and parse strings

// these are static :
int AccessPoint::msgLength;
String AccessPoint::msg[MAX_TOTAL_CONFIG_LENGTH];
Config *AccessPoint::config = NULL;
ESP8266WebServer *AccessPoint::webServer = NULL;
WebSocketsServer *AccessPoint::socketServer = NULL;

//======================= websocket server callback ==========================//

static void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: {
        // notify disconnection (with blink) ?
      }
      break;
    case WStype_CONNECTED: {
        // IPAddress ip = AccessPoint::socketServer->remoteIP(num);
        AccessPoint::encodeAndSendSettings(num);
      }
      break;
    case WStype_TEXT: {
        AccessPoint::parseInputMessage(payload, length);
      }
      break;
  }
}

//=========================== AccessPoint methods ============================//

void
AccessPoint::init(Config *c) {
  digitalWrite(pinLedWifi, HIGH);

  config = c;
  config->init();
  config->load(); // read from file if exists, otherwise use default values

  char apssid[30];
  strcpy(apssid, "movuino-");
  strcat(apssid, config->getMovuinoId());

  // Don't use WIFI_AP_STA mode !!!!!
  // (or be warned it also tries to connect to the last network the ESP
  // remembers and prevents websockets to work if it fails)

  WiFi.mode(WIFI_AP);
  IPAddress ip(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(ip, gateway, subnet);

  if (WiFi.softAP(apssid)) {
    initialized = true;
  }

  // servers

  webServer = new ESP8266WebServer(80);
  socketServer = new WebSocketsServer(81);

  socketServer->begin();
  socketServer->onEvent(webSocketEvent);

  // serve main config page

  webServer->on("/", []() {
    webServer->send(200, "text/html", configWebPage);
  });

  // just in case we change the config file structure,
  // here is a way to force it to be cleared :

  webServer->on("/clear", []() {
    config->reset();

    if (SPIFFS.exists("/config.txt")) {
      webServer->send(200, "text/plain", "config file not cleared !");
    } else {
      webServer->send(200, "text/plain", "config file cleared !");
      config->store();
    }
  });

  webServer->begin();
}

void
AccessPoint::update() {
  socketServer->loop();
  webServer->handleClient();

  digitalWrite(pinLedWifi, initialized ? LOW : HIGH);
}

//----------------- executed by websocket server callback --------------------//

void
AccessPoint::encodeAndSendSettings(uint8_t num, bool broadcast) {
  char packet[MAX_TOTAL_CONFIG_STRING_SIZE];
  String intStr;

  strcpy(packet, "settings\n");

  strcat(packet, config->getSsid());
  strcat(packet, "\n");
  strcat(packet, config->getPassword());
  strcat(packet, "\n");
  strcat(packet, config->getHostIP());
  strcat(packet, "\n");

  intStr = String(config->getInputPort());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  intStr = String(config->getOutputPort());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");

  intStr = String(config->getAccelRange());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  intStr = String(config->getGyroRange());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");

  intStr = String(config->getUseWiFi());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  intStr = String(config->getUseSerial());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  intStr = String(config->getSendSingleFrame());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");

  intStr = String(config->getReadMagPeriod());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  intStr = String(config->getOutputFramePeriod());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");
  intStr = String(config->getButtonHoldDuration());
  strcat(packet, intStr.c_str());
  strcat(packet, "\n");

  strcat(packet, config->getMovuinoId());
  strcat(packet, "\n");
  String version("v");
  version += MOVUINO_FIRMWARE_VERSION_MAJOR;
  version += ".";
  version += MOVUINO_FIRMWARE_VERSION_MINOR;
  version += ".";
  version += MOVUINO_FIRMWARE_VERSION_PATCH;
  strcat(packet, version.c_str());
  strcat(packet, "\n");

  if (broadcast) {
    socketServer->broadcastTXT(packet);
  } else {
    socketServer->sendTXT(num, packet);
  }
}

void
AccessPoint::parseInputMessage(uint8_t *payload, size_t length) {
  size_t index = 0;

  msgLength = 0;
  msg[0] = String("");

  for (size_t index = 0; index < length; index++) {
    if (payload[index] != '\n') {
      msg[msgLength] += (char) payload[index];
    } else if (index < length - 1) {
      msgLength++;
      if (msgLength == MAX_TOTAL_CONFIG_LENGTH) break;
      msg[msgLength] = String("");
    }
  }

  msgLength++;
  processInputMessage();
}

void
AccessPoint::processInputMessage() {
  bool light = false;
  for (int i = 0; i < 10; i++) {
    light = !light;
    digitalWrite(pinLedWifi, light ? LOW : HIGH);
    delay(50);
  }

  if (msgLength > 0) {
    if (msg[0] == "clear") {
      config->reset();
      config->store();
      encodeAndSendSettings(0, true);
    } else if (msg[0] == "settings" && msgLength >= 14) {
      config->setSsid(msg[1].c_str());
      config->setPassword(msg[2].c_str());
      config->setHostIP(msg[3].c_str());
      config->setInputPort(msg[4].toInt());
      config->setOutputPort(msg[5].toInt());
      config->setAccelRange(msg[6].toInt());
      config->setGyroRange(msg[7].toInt());
      config->setUseWiFi(msg[8].toInt() > 0);
      config->setUseSerial(msg[9].toInt() > 0);
      config->setSendSingleFrame(msg[10].toInt() > 0);
      config->setReadMagPeriod(msg[11].toInt());
      config->setOutputFramePeriod(msg[12].toInt());
      config->setButtonHoldDuration(msg[13].toInt());
      config->store();
    } else {
      // ... something else needed ?
    }
  }
}

/*
void
AccessPoint::decodeAndStoreSettings(uint8_t *payload, size_t length) {
  size_t index = 0;
  size_t charCnt = 0;
  size_t wordCnt = 0;

  APCommand command = APUnknown;
  char element[length];

  while (index < length) {
    if (payload[index] != '\n') {
      element[charCnt++] = (char) payload[index];
    } else {
      element[charCnt] = '\0';
      if (wordCnt == 0) {
        if (strcmp((const char *)element, "clear") == 0) {
          bool light = false;
          for (int i = 0; i < 10; i++) {
            light = !light;
            digitalWrite(pinLedWifi, light ? LOW : HIGH);
            delay(50);
          }

          command = APClear;
          config->reset();
          config->store();
          //return;

        } else if (strcmp((const char *)element, "settings") == 0) {
          command = APSettings;
        } else {
          //return;
        }
      } else if (command == APSettings) {
        if (wordCnt == 1) {
          config->setSsid(element);
        } else if (wordCnt == 2) {
          config->setPassword(element);
        } else if (wordCnt == 3) {
          config->setHostIP(element);
        } else if (wordCnt == 4) {
          config->setInputPort(String(element).toInt());
        } else if (wordCnt == 5) {
          config->setOutputPort(String(element).toInt());
        } else if (wordCnt == 6) {
          config->setAccelRange(String(element).toInt());
        } else if (wordCnt == 7) {
          config->setGyroRange(String(element).toInt());
        } else if (wordCnt == 8) {
          config->setUseWiFi(String(element).toInt() > 0);
        } else if (wordCnt == 9) {
          config->setUseSerial(String(element).toInt() > 0);
        } else if (wordCnt == 10) {
          config->setSendSingleFrame(String(element).toInt() > 0);
        } else if (wordCnt == 11) {
          config->setReadMagPeriod(String(element).toInt());
        } else if (wordCnt == 12) {
          config->setOutputFramePeriod(String(element).toInt());
        } else if (wordCnt == 13) {
          config->setButtonHoldDuration(String(element).toInt());

          bool light = false;
          for (int i = 0; i < 10; i++) {
            light = !light;
            digitalWrite(pinLedWifi, light ? LOW : HIGH);
            delay(50);
          }

          config->store();
          //return;
        }
      }

      charCnt = 0;
      wordCnt++;
    }

    index++;
  }
}
//*/
