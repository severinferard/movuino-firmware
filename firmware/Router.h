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

#include <Arduino.h>

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

  // bool stationMode;
  bool useWiFi;
  bool useSerial;
  bool readMag;
  bool useNeoPixelAsIndicator;

  int outputOSCPeriod;
  int connectionState;

  OSCMessage outputOSCMessage;
  char arg[MAX_OSC_STRING_ARG_LENGTH];

  OSCMessage outputFrameMessage;
  unsigned long lastFrameOutputDate;

public:
  Router() {}
  ~Router() {}

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

    // stationMode = config->getStationMode();
    useWiFi = config->getUseWiFi();
    useSerial = config->getUseSerial();
    readMag = config->getReadMag();
    useNeoPixelAsIndicator = config->getUseNeoPixelAsIndicator();
    outputOSCPeriod = config->getOSCOutputPeriod();

    connectionState = 0;

    indic->setUseNeoPixel(useNeoPixelAsIndicator);

    ////////// mpu stuff

    imu->init();
    imu->setAccelRange(config->getAccelRange());
    imu->setGyroRange(config->getGyroRange());
    imu->enableMag(readMag);

    ////////// serial stuff

    addConfigOSCMessageListeners(serial);

    if (useSerial) {
      addNormalOSCMessageListeners(serial);
    }

    serial->start();

    ////////// wifi stuff

    wifi->setUdpInputPort(config->getUdpInputPort());
    wifi->setUdpOutputPort(config->getUdpOutputPort());
    wifi->setHostIP(config->getHostIP());
    wifi->setStaticIP(config->getStaticIP(), false);
    wifi->setGatewayIP(config->getGatewayIP());
    wifi->setSubnetIP(config->getSubnetIP());

    wifi->setConnectionStateListener([this](WiFiConnectionState s) {
      onWiFiConnectionState(s);
    });

    outputFrameMessage.setAddress("/movuino");

    if (useWiFi) {
      addNormalOSCMessageListeners(wifi);
      startWiFi();
    }
  }

  void onWiFiConnectionState(WiFiConnectionState s) {
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

    outputOSCMessage.setAddress("/wifi/state");
    outputOSCMessage.empty();
    outputOSCMessage.add(connectionState);
    serial->sendOSCMessage(outputOSCMessage);    
  }

  void update() {

    indic->update();
    neopix->update();
    vibro->update();
    button->update();

    serial->update();
    if (useWiFi) { wifi->update(); }

    unsigned long now = millis();

    if (now - lastFrameOutputDate >= outputOSCPeriod) {

      ////////// SEND OSC FRAME !!!!!

      lastFrameOutputDate = now;

      imu->update();
      outputFrameMessage.empty();
      outputFrameMessage.add(config->getMovuinoID());
      outputFrameMessage.add(imu->getAccelX());
      outputFrameMessage.add(imu->getAccelY());
      outputFrameMessage.add(imu->getAccelZ());
      outputFrameMessage.add(imu->getGyroX());
      outputFrameMessage.add(imu->getGyroY());
      outputFrameMessage.add(imu->getGyroZ());
      outputFrameMessage.add(imu->getMagX());
      outputFrameMessage.add(imu->getMagY());
      outputFrameMessage.add(imu->getMagZ());

      ButtonState s = button->getState();
      int val = 0;
      if (s == ButtonStatePressed) {
        val = 1;
      } else if (s == ButtonStateReleased) {
        val = 0;
      } else if (s == ButtonStateHolding) {
        val = 2;
      }
      outputFrameMessage.add(val);

      outputFrameMessage.add(vibro->isVibrating() ? 1 : 0);

      if (useSerial) serial->sendOSCMessage(outputFrameMessage);
      if (useWiFi) wifi->sendOSCMessage(outputFrameMessage);
    }
  }

private:
  void startWiFi() {
    if (config->getStationMode()) {
      wifi->enableDHCP(true);
      wifi->startWiFi(WIFI_STA, config->getStaSsid(), config->getStaPass());
    } else {
      wifi->enableDHCP(false);
      wifi->startWiFi(WIFI_AP, config->getApSsid(), config->getApPass());
    }    
  }

  ////////// "normal" osc messages

  void addNormalOSCMessageListeners(OSCInterface *interface) {
    interface->addOSCMessageListener("/vibro/pulse", [&, interface](OSCMessage& msg) {
      if (msg.size() > 2) {
        vibro->pulse(msg.getInt(0), msg.getInt(1), msg.getInt(2));
      }
    });

    interface->addOSCMessageListener("/vibro/now", [&, interface](OSCMessage& msg) {
      if (msg.size() > 0) {
        vibro->vibrate(msg.getInt(0) > 0);
      }
    });

    interface->addOSCMessageListener("/neopix", [&, interface](OSCMessage& msg) {
      if (useNeoPixelAsIndicator && msg.size() == 3) {
        neopix->setColor(msg.getInt(0), msg.getInt(1), msg.getInt(2));
      }
    });
  }

  void removeNormalOSCMessageListeners(OSCInterface *interface) {
    interface->removeOSCMessageListener("/vibro/pulse");
    interface->removeOSCMessageListener("/vibro/now");
    interface->removeOSCMessageListener("/vibro/neopix");
  }

  ////////// configuration OSC messages

  void addConfigOSCMessageListeners(OSCInterface *interface) {
    interface->addOSCMessageListener("/hello", [&, interface](OSCMessage& msg) {
      msg.empty();
      msg.add("movuino");
      msg.add(wifi->getMovuinoUUID());
      msg.add(connectionState);
      msg.add(WiFi.localIP().toString().c_str());
      msg.add(config->getMovuinoID());
      msg.add(config->getFirmwareVersion());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/id/get", [&, interface](OSCMessage& msg) {
      msg.empty();
      msg.add(config->getMovuinoID());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/id/set", [&, interface](OSCMessage& msg) {
      if (msg.size() > 0) {
        msg.getString(0, static_cast<char *>(arg), MAX_OSC_STRING_ARG_LENGTH);
        if (strcmp(config->getMovuinoID(), static_cast<const char *>(arg)) != 0) {
          config->setMovuinoID(static_cast<const char *>(arg));
          config->store();
        }
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/serial/enable", [&, interface](OSCMessage& msg) {
      if (msg.size() > 0) {
        bool e = msg.getInt(0) > 0;
        if (e != useSerial) {
          useSerial = e;

          OSCInterface *s = static_cast<OSCInterface *>(serial);
          if (useSerial) {
            addNormalOSCMessageListeners(s);
          } else {
            removeNormalOSCMessageListeners(s);
          }
          
          config->setUseSerial(useSerial);
          config->store();
        }
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/magneto/enable", [&, interface](OSCMessage& msg) {
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

    interface->addOSCMessageListener("/wifi/enable", [&, interface](OSCMessage& msg) {
      if (msg.size() > 0) {
        bool e = msg.getInt(0) > 0;
        if (e != useWiFi) {
          useWiFi = e;

          OSCInterface *s = static_cast<OSCInterface *>(wifi);
          if (useWiFi) {
            addNormalOSCMessageListeners(s);
            startWiFi();
          } else {
            wifi->stopWiFi();
            removeNormalOSCMessageListeners(s);
          }

          config->setUseWiFi(useWiFi);
          config->store();
        }
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/wifi/get", [&, interface](OSCMessage& msg) {
      msg.empty();
      msg.add(config->getStaSsid());
      msg.add(config->getStaPass());
      msg.add(config->getHostIP().toString().c_str());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/wifi/set", [&, interface](OSCMessage& msg) {
      if (msg.size() > 1) {
        msg.getString(0, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
        config->setStaSsid((const char *)arg);

        if (msg.size() == 2) { // no password
          config->setStaPass("");
          msg.getString(1, static_cast<char *>(arg), MAX_OSC_STRING_ARG_LENGTH);
        } else { // msg.size() >= 3, we have a password
          msg.getString(1, static_cast<char *>(arg), MAX_OSC_STRING_ARG_LENGTH);
          config->setStaPass(static_cast<const char *>(arg));
          msg.getString(2, static_cast<char *>(arg), MAX_OSC_STRING_ARG_LENGTH);
        }

        IPAddress ip;
        ip.fromString(static_cast<const char *>(arg));
        wifi->setHostIP(ip);
        config->setHostIP(ip);
        config->store();
        interface->sendOSCMessage(msg);

        if (useWiFi) {
          startWiFi();
        }
      }
    });

    interface->addOSCMessageListener("/ap/get", [&, interface](OSCMessage& msg) {
      msg.empty();
      msg.add(config->getStationMode() ? 0 : 1);
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/ap/set", [&, interface](OSCMessage& msg) {
      if (msg.size() > 0) {
        config->setStationMode(msg.getInt(0) == 0);
        config->store();
        interface->sendOSCMessage(msg);

        if (useWiFi) {
          startWiFi();
        }
      }
    });

    interface->addOSCMessageListener("/ports/get", [&, interface](OSCMessage& msg) {
      msg.empty();
      msg.add(config->getUdpInputPort());
      msg.add(config->getUdpOutputPort());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/ports/set", [&, interface](OSCMessage& msg) {
      if (msg.size() > 1) {
        config->setUdpInputPort(msg.getInt(0));
        config->setUdpOutputPort(msg.getInt(1));
        config->store();
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/ranges/get", [&, interface](OSCMessage& msg) {
      msg.empty();
      msg.add(config->getAccelRange());
      msg.add(config->getGyroRange());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/ranges/set", [&, interface](OSCMessage& msg) {
      if (msg.size() > 1) {
        config->setAccelRange(msg.getInt(0));
        config->setGyroRange(msg.getInt(1));
        imu->setAccelRange(config->getAccelRange());
        imu->setGyroRange(config->getGyroRange());
        config->store();
        interface->sendOSCMessage(msg);
      }
    });

    interface->addOSCMessageListener("/frameperiod/get", [&, interface](OSCMessage& msg) {
      msg.empty();
      msg.add(config->getOSCOutputPeriod());
      interface->sendOSCMessage(msg);
    });

    interface->addOSCMessageListener("/frameperiod/set", [&, interface](OSCMessage& msg) {
      if (msg.size() > 0) {
        config->setOSCOutputPeriod(msg.getInt(0));
        config->store();
        interface->sendOSCMessage(msg);
        outputOSCPeriod = config->getOSCOutputPeriod();
      }
    });
  }
};

#endif /* _MOVUINO_ROUTER_H_ */