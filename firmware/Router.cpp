#include "Config.h"
#include "Button.h"
#include "Sensors.h"
#include "Vibrator.h"
#include "SerialInterface.h"
#include "WiFiInterface.h"
#include "Router.h"

void
Router::init(Config *c, Button *b, Sensors *s, Vibrator *v, SerialInterface *si, WiFiInterface *wi) {
  config = c;
  config->init();
  config->load(); // read from file if exists, otherwise use default values

  initOSCAddresses(config->getMovuinoId());

  button = b;
  button->init(config, this);

  sensors = s;
  sensors->init(config, this);

  vibrator = v;
  vibrator->init(config, this);

  serial = si;
  serial->init(config, this);

  wifi = wi;
  wifi->init(config, this);
}

void
Router::update() {
  serial->update();
  wifi->update();
  vibrator->update();
  button->update();
  sensors->update();
}

void
Router::routeWiFiMessage(OSCMessage& msg) {
  routeOSCMessage(msg);
}

void
Router::wiFiMessageErrorCallback(OSCMessage& msg) {
  // TODO
}

void
Router::routeSerialMessage(OSCMessage& msg) {
  routeOSCMessage(msg);
}

void
Router::serialMessageErrorCallback(OSCMessage&msg) {
  // TODO
}

//--------------------------------- EVENTS -----------------------------------//

void
Router::onWiFiConnectionEvent(WiFiConnectionState s) {
  int val = 0;

  if (s == WiFiConnecting) { // send 2
    val = 2;
  } else if (s == WiFiConnected) {
    val = 1;
  } else if (s == WiFiDisconnected) {
    val = 0;
  }

  sendWiFiConnectionMessage(val);
}

void
Router::onButtonEvent(ButtonState s) {
  if (!config->getSendSingleFrame()) {
    sendButtonMessage(getButtonIntValue(s));
  }
}

void
Router::onNewSensorValues(float *f) {
  if (config->getSendSingleFrame()) {
    sendSingleFrame(f);
  } else {
    sendSensorsMessage(f);
  }
}

//=============================== PRIVATE ====================================//

int
Router::getButtonIntValue(ButtonState s) {
  int val = 0;

  if (s == ButtonPressed) { // send 2
    val = 1;
  } else if (s == ButtonReleased) {
    val = 0;
  } else if (s == ButtonHolding) {
    val = 2;
  }

  return val;
}

void
Router::routeOSCMessage(OSCMessage& msg) {
  char address[MAX_OSC_ADDRESS_LENGTH];
  char arg[MAX_OSC_STRING_ARG_LENGTH];

  msg.getAddress(address);

  //----------------------------------------------------------------------------
  if (strcmp(address, oscAddresses[oscInputWiFiEnable]) == 0) { // usually from serial
    config->setUseWiFi(msg.getInt(0) > 0);
    config->store();

    sendSerialMessage(msg);
    sendWiFiMessage(msg);

    if (config->getUseWiFi()) {
      wifi->startWiFi();
    } else {
      wifi->stopWiFi();
    }
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputSetWiFi]) == 0) {
    // we get ssid, password, hostIP, portIn, portOut
    msg.getString(0, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
    config->setSsid((const char *)arg);
    msg.getString(1, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
    config->setPassword((const char *)arg);
    msg.getString(2, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
    config->setHostIP((const char *)arg);

    config->setInputPort(msg.getInt(3));
    config->setOutputPort(msg.getInt(4));

    config->store();

    sendSerialMessage(msg);
    sendWiFiMessage(msg);

    wifi->startWiFi(); // will check if useWifi is set
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputGetWiFi]) == 0) {
    sendWiFiSettings(oscInputGetWiFi);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputSetRange]) == 0) {
    sensors->setAccelRange(msg.getInt(0));
    sensors->setGyroRange(msg.getInt(1));

    config->setAccelRange(msg.getInt(0));
    config->setGyroRange(msg.getInt(1));

    config->store();

    // mirror
    sendSerialMessage(msg);
    sendWiFiMessage(msg);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputGetRange]) == 0) {
    sendAccelGyroRanges();
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputSetConfig]) == 0) {
    sensors->setReadMagPeriod(msg.getInt(2));
    sensors->setOutputFramePeriod(msg.getInt(3));

    config->setUseSerial(msg.getInt(0) > 0);
    config->setSendSingleFrame(msg.getInt(1) > 0);
    config->setReadMagPeriod(msg.getInt(2));
    config->setOutputFramePeriod(msg.getInt(3));
    config->setButtonHoldDuration(msg.getInt(4));

    config->store();

    sendSerialMessage(msg);
    sendWiFiMessage(msg);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputGetConfig]) == 0) {
    sendGlobalConfig();
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputSetAll]) == 0) {
  // TODO
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputGetAll]) == 0) {
  // TODO
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputVibroPulse]) == 0) {
    vibrator->pulse(
      (unsigned long) msg.getInt(0),
      (unsigned long) msg.getInt(1),
      (unsigned long) msg.getInt(2)
    );
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputVibroNow]) == 0) {
    vibrator->vibrate(msg.getInt(0) != 0);
  //----------------------------------------------------------------------------
  }
}

void
Router::sendWiFiConnectionMessage(int i) {
  OSCMessage msg(oscAddresses[oscOutputWiFiState]);
  msg.add(i);

  sendSerialMessage(msg);
}

void
Router::sendWiFiSettings(oscAddress a) {
  OSCMessage msg(oscAddresses[a]);
  msg.add(config->getSsid());
  msg.add(config->getPassword());
  msg.add(config->getHostIP());
  msg.add(config->getInputPort());
  msg.add(config->getOutputPort());

  sendSerialMessage(msg);
}

void
Router::sendAccelGyroRanges() {
  OSCMessage msg(oscAddresses[oscOutputSetRange]);
  msg.add(config->getAccelRange());
  msg.add(config->getGyroRange());

  sendSerialMessage(msg);
}

void
Router::sendGlobalConfig() {
  OSCMessage msg(oscAddresses[oscOutputSetConfig]);
  msg.add(config->getUseSerial() ? "1" : "0");
  msg.add(config->getSendSingleFrame() ? "1" : "0");
  msg.add(config->getReadMagPeriod());
  msg.add(config->getOutputFramePeriod());
  msg.add(config->getButtonHoldDuration());

  sendSerialMessage(msg);
}

void
Router::sendButtonMessage(int i) {
  OSCMessage msg(oscAddresses[oscOutputButton]);
  msg.add(i);

  if (config->getUseSerial()) {
    sendSerialMessage(msg);
  }

  if (config->getUseWiFi()) {
    sendWiFiMessage(msg);
  }
}

void
Router::sendSensorsMessage(float *f) {
  OSCMessage msg(oscAddresses[oscOutputSensors]);

  for (unsigned int i = 0; i < 9; ++i) {
    msg.add(*(f + i));
  }

  if (config->getUseSerial()) {
    sendSerialMessage(msg);
  }

  if (config->getUseWiFi()) {
    sendWiFiMessage(msg);
  }
}

void
Router::sendSingleFrame(float *f) {
  OSCMessage msg(oscAddresses[oscOutputFrame]);

  for (unsigned int i = 0; i < 9; ++i) {
    msg.add(*(f + i));
  }

  msg.add(getButtonIntValue(button->getState())); // append the button value
  msg.add(vibrator->isVibrating() ? 1 : 0);
  msg.add(wifi->getStringIPAddress().c_str());

  if (config->getUseSerial()) {
    sendSerialMessage(msg);
  }

  if (config->getUseWiFi()) {
    sendWiFiMessage(msg);
  }
}

void
Router::sendWiFiMessage(OSCMessage& msg/*, bool critical = false*/) {
  wifi->sendMessage(msg, config->getHostIP(), config->getOutputPort());
}

void
Router::sendSerialMessage(OSCMessage& msg) {
  serial->sendMessage(msg);
}
