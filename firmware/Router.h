#ifndef _MOVUINO_ROUTER_H_
#define _MOVUINO_ROUTER_H_

// #include <OSCMessage.h>

// class ConfigFile;
// class IndicatorLight;
// class NeoPixel;
// class Vibrator;
// class Button;
// class IntegratedSensors;
// class SerialInterface;
// class WiFiInterface;

#include "ConfigFile.h"
#include "IndicatorLight.h"
#include "NeoPixel.h"
#include "Vibrator.h"
#include "Button.h"
#include "IntegratedSensors.h"
#include "SerialInterface.h"
#include "WiFiInterface.h"

#include "globals.h"

class Router {
private:
  ConfigFile *config;
  IndicatorLight *indic;
  NeoPixel *neopix;
  Vibrator *vibro;
  Button *button;
  IntegratedSensors *imu;
  SerialInterface *serial;
  WiFiInterface *wifi;

  bool useSerial;
  bool useWiFi;
  bool readMag;
  bool useNeoPixelAsIndicator;

  int connectionState;

  OSCMessage outputOSCMessage;
  char arg[MAX_OSC_STRING_ARG_LENGTH];

public:
  Router();
  ~Router();

  void init(ConfigFile *config, IndicatorLight *indic, NeoPixel *neopix,
            Vibrator *vibro, Button *button, IntegratedSensors *imu,
            SerialInterface *serial, WiFiInterface *wifi) {
    this->config = config;
    this->indic = indic;
    this->neopix = neopix;
    this->vibro = vibro;
    this->button = button;
    this->imu = imu;
    this->serial = serial;
    this->wifi = wifi;

    useSerial = config->getUseSerial();
    useWiFi = config->getUseWiFi();
    readMag = config->getReadMag();
    useNeoPixelAsIndicator = config->getUseNeoPixelAsIndicator();
    outputOSCPeriod = config->getOutputOSCPeriod();

    connectionState = 0;

    indic->setUseNeoPixel(useNeoPixelAsIndicator);

    ////////// mpu stuff

    imu->init();
    imu->setAccelRange(config->getAccelRange());
    imu->setGyroRange(config->getGyroRange());
    imu->enableMag(readMag);

    ////////// serial stuff

    addConfigOSCMessageListeners(static_cast<OSCInterface *>(serial));

    if (useSerial) {
      addNormalOSCMessageListeners(static_cast<OSCInterface *>(serial));
    }

    serial->start();

    ////////// wifi stuff

    wifi->enableDHCP(config->getDHCPOn());
    wifi->setHostIP(config->getHostIP());
    wifi->setStaticIP(config->getStaticIP());
    wifi->setGatewayIP(config->getGatewayIP());
    wifi->setSubnetIP(config->getSubnetIP());

    wifi->setConnectionStateListener([&](WiFiConnectionState s) {
      outputOSCMessage.empty();
      outputOSCMessage.setAddress("/wifi/state");

      switch (s) {
        case WiFiDisconnected:
          indic->setLow();
          connectionState = 0;
          break;
        case WiFiConnecting:
          indic->setPeriod(LOW_BLINK_PERIOD);
          connectionState = 2;
          break;
        case WiFiConnected:
          indic->setHigh();
          connectionState = 1;
          break;
        default:
          break;
      }

      indic->update();
      outputOSCMessage.add(connectionState);
      serial->sendOSCMessage(outputOSCMessage);
    });

    if (useWiFi) {
      addNormalOSCMessageListeners(static_cast<OSCInterface *>(wifi));
      startWiFi();
    }
  }

  void update() {
    indic->update();
    imu->update();

    serial->update();
    if (useWiFi) { wifi->update(); }

    if ()
  }

private:
  void startWiFi() {
    if (config->getStationMode()) {
      wifi->start(WIFI_STA, config->getStaSsid(), config->getStaPass());
    } else {
      wifi->start(WIFI_AP, config->getApSsid(), config->getApPass());
    }    
  }

  ////////// "normal" osc messages

  void addNormalOSCMessageListeners(OSCInterface *interface) {
    interface->assOSCMessageListener("/vibro/pulse", [&](OSCMessage& msg) {
      if (msg.size() > 2) {
        vibro->pulse(msg.getInt(0), msg.getInt(1), msg.getInt(2));
      }
    });

    interface->assOSCMessageListener("/vibro/now", [&](OSCMessage& msg) {
      if (msg.size() > 0) {
        vibro->vibrate(msg.getInt(0) > 0);
      }
    });

    interface->assOSCMessageListener("/neopix", [&](OSCMessage& msg) {
      if (useNeoPixelAsIndicator && msg.size() == 3) {
        neopix->setColor(msg.getInt(0), msg.getInt(1), msg.getInt(2));
      }
    });
  }

  void removeNormalOSCMessageListeners(OSCMessage *interface) {
    interface->removeOSCMessageListener("/vibro/pulse");
    interface->removeOSCMessageListener("/vibro/now");
    interface->removeOSCMessageListener("/vibro/neopix");
  }

  ////////// configuration OSC messages

  void addConfigOSCMessageListeners(OSCInterface *interface) {
    interface->addOSCMessageListener("/hello", [&](OSCMessage& msg) {
      msg.empty();
      msg.add("movuino");
      msg.add(wifi->getMovuinoUUID());
      msg.add(connectionState);
      msg.add(WiFi.localIP().toString().c_str());
      msg.add(config->getMovuinoId());
      msg.add(config->getFirmwareVersion());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/id/get", [&](OSCMessage& msg) {
      msg.empty();
      msg.add(config->getMovuinoID());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/id/set", [&](OSCMessage& msg) {
      if (msg.size() > 0) {
        msg.getString(0, static_cast<char *>(arg), MAX_OSC_STRING_ARG_LENGTH);
        if (strcmp(config->getMovuinoID(), static_cast<const char *>(arg)) != 0) {
          config->setMovuinoID(static_cast<const char *>(arg));
          config->store();
        }
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/serial/enable", [&](OSCMessage& msg) {
      if (msg.size() > 0) {
        bool e = msg.getInt(0) > 0;
        if (e != useSerial) {
          useSerial = e;

          if (useSerial) { addNormalOSCMessageListeners(serial); }
          else { removeNormalOSCMessageListeners(serial); }
          
          config->setUseSerial(enableSerial);
          config->store();
        }
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/magneto/enable", [&](OSCMessage& msg) {
      if (msg.size() > 0) {
        bool e = msg.getInt(0) > 0;
        if (e != readMag) {
          readMag = e;
          imu->enableMag(readMag);
          config->setReadMag(readMag);
          config->store();
        }
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/wifi/enable", [&](OSCMessage& msg) {
      if (msg.size() > 0) {
        bool e = msg.getInt(0) > 0;
        if (e != useWiFi) {
          useWiFi = e;

          if (useWiFi) { startWiFi(); }
          else { wifi->stopWiFi(); }

          config->setUseWiFi(useWiFi);
          config->store();
        }
      }
    });

    interface->addOSCMessageListener("/wifi/get", [&](OSCMessage& msg) {

    });

    interface->addOSCMessageListener("/wifi/set", [&](OSCMessage& msg) {

    });

    interface->addOSCMessageListener("/ports/get", [&](OSCMessage& msg) {
      msg.empty();
      msg.add(config->getUdpInputPort());
      msg.add(config->getUdpOutputPort());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/ports/set", [&](OSCMessage& msg) {
      if (msg.size() > 1) {
        config->setUdpInputPort(msg.getInt(0));
        config->setUdpOutputPort(msg.getInt(1));
        config->store();
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/ranges/get", [&](OSCMessage& msg) {
      msg.empty();
      msg.add(config->getAccelRange());
      msg.add(config->getGyroRange());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/ranges/set", [&](OSCMessage& msg) {
      if (msg.size() > 1) {
        config->setAccelRange(msg.getInt(0));
        config->setGyroRange(msg.getInt(1));
        config->store();
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/frameperiod/get", [&](OSCMessage& msg) {

    });

    interface->addOSCMessageListener("/frameperiod/set", [&](OSCMessage& msg) {

    });
  }
};

#endif /* _MOVUINO_ROUTER_H_ */