#include "Router.h"
#include "Config.h"
#include "WiFiInterface.h"


void
WiFiInterface::setBootMode(WiFiBootMode m) {
  wifiBootMode = m;
}

void
WiFiInterface::init(Config *c, Router *r) {
  config = c;
  router = r;

  if (wifiBootMode == WiFiAccessPoint ||
      (wifiBootMode == WiFiStation && config->getUseWiFi())) {
    startWiFi();
  }
}

//============================ TIMER METHODS =================================//

void
WiFiInterface::update() {
  Timer::update();

  digitalWrite(pinLedWifi, wifiLight ? LOW : HIGH);
  digitalWrite(pinLedBat, batLight ? LOW : HIGH);

  if (WiFi.status() == WL_CONNECTED) {
    readMessages();
  }
}

void
WiFiInterface::callback() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiLight = !wifiLight;
  } else {
    stop();
  }
}

void
WiFiInterface::stop() {
  Timer::stop();

  if (WiFi.status() == WL_CONNECTED) {
    initialized = true;
    onConnectionEvent(WiFiConnected);
  } else {
    stopWiFi();
    onConnectionEvent(WiFiDisconnected);
  }
}

//============================ WIFIINTERFACE =================================//

void
WiFiInterface::readMessages(/*Router *router*/) {
  OSCMessage msg;
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    while (packetSize--) {
      msg.fill(udp.read()); // read incoming message into the bundle
    }
    if (!msg.hasError()) {
      router->routeWiFiMessage(msg);
    } else {
      router->wiFiMessageErrorCallback(msg);
    }
  }
}

bool
WiFiInterface::sendMessage(OSCMessage& msg, const char *hostIP, int portOut) {
  if (WiFi.status() == WL_CONNECTED) {
    udp.beginPacket(hostIP, portOut); // send message to computer target with "hostIP" on "port"
    msg.send(udp);
    udp.endPacket();
    return true;
  }

  return false;
}

//============================= WIFI CONNECTION ==============================//

bool
WiFiInterface::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

//---------------------------------- START -----------------------------------//

void
WiFiInterface::startWiFi() {
  wifiLight = batLight = true;

  if (wifiBootMode == WiFiAccessPoint) {
    if (!initialized) {
      char apssid[30];
      strcpy(apssid, "movuino-");
      strcat(apssid, config->getMovuinoId());

      if (WiFi.softAP(apssid, "")) {
        initialized = true;
        // digitalWrite(pinLedWifi, LOW); // turn ON wifi led
      } else {
        wifiLight = batLight = false;
        // digitalWrite(pinLedWifi, HIGH); // turn OFF wifi led
      }
    }
  } else if (config->getUseWiFi()) { // wifiBootMode == WiFiStation
    if (!initialized) {
      WiFi.mode(WIFI_STA);
      initialized = true;
    } else {
      WiFi.forceSleepWake();
    }

    WiFi.begin(config->getSsid(), config->getPassword());
    // digitalWrite(pinLedBat, LOW); // turn ON battery led

    // this used to be in startConnectionTimer :
    start(connectionTimeout);
    onConnectionEvent(WiFiConnecting);
  } else {
    // do nothing except switch off lights because useWiFi is false
    wifiLight = batLight = false;
  }
}

//-------------------------------- SHUTDOWN ----------------------------------//

void
WiFiInterface::stopWiFi() {
  if (wifiBootMode == WiFiAccessPoint) return; // reboot instead of doing this

  if (running) {
    running = false;
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);
  }

  wifiLight = batLight = false;
}

//---------------------------------- AWAKE -----------------------------------//

void
WiFiInterface::toggleWiFiState() {
  if (WiFi.status() == WL_CONNECTED) {
    stopWiFi();
  } else {
    startWiFi();
  }
}

void
WiFiInterface::onConnectionEvent(WiFiConnectionState s) {
  if (s == WiFiConnected) {
    // Start client port (to send message)
    udp.begin(config->getOutputPort());
    delay(50); // ok
    // Start server port (to receive message)
    udp.begin(config->getInputPort());
    wifiLight = true;
    digitalWrite(pinLedWifi, LOW); // turn ON wifi led
  }

  router->onWiFiConnectionEvent(s);
}

//=============================== UTILITIES ==================================//

String
WiFiInterface::getStringMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);

  String res(mac[0], HEX);
  for (int i = 1; i < 6; i++) {
    res += ":";
    res += String(mac[i], HEX);
  }
  return res;
}

void
WiFiInterface::getIPAddress(int *res) { // must be of type int[4]
  // sometimes we get weird ip address values, if it happens again check here :
  if (isConnected()) {
    IPAddress ip = WiFi.localIP();
    for (int i = 0; i < 4; i++) {
      *(res + i) = ip[i];
    }
  }
}

String
WiFiInterface::getStringIPAddress() {
  int ip[4] = { 0, 0, 0, 0 };
  getIPAddress(&ip[0]);
  String sip = String(ip[0]);
  sip += ".";
  sip += ip[1];
  sip += ".";
  sip += ip[2];
  sip += ".";
  sip += ip[3];
  return sip;
}

