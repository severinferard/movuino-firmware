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
Router::serialMessageErrorCallback(OSCMessage& msg) {
  // TODO
}

//--------------------------------- EVENTS -----------------------------------//

void
Router::onWiFiConnectionEvent(WiFiConnectionState s) {
  sendWiFiConnectionMessage(getConnectionStateOSCValue(s));
}

void
Router::onButtonEvent(ButtonState s) {
  // if (!config->getSendSingleFrame()) {
  //   sendButtonMessage(getButtonIntValue(s));
  // }

  sendButtonMessage(getButtonIntValue(s)); // send button anyways
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

  int msgLength = msg.size();
  msg.getAddress(address);

  //----------------------------------------------------------------------------
  if (strcmp(address, oscAddresses[oscInputHello]) == 0 && msgLength == 0) { // usually from serial
    // respond with "movuino", id, firmware version, wifistate
    sayHello();
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputWiFiEnable]) == 0 && msgLength > 0) { // usually from serial
    config->setUseWiFi(msg.getInt(0) > 0);
    config->store();

    sendSerialMessage(msg);
    sendWiFiMessage(msg);

    wifi->stopWiFi();
    wifi->startWiFi();// will check if useWifi is set
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputSetWiFi]) == 0 && msgLength > 1) {
    // we can have :
    // <ssid> <hostIP> if no password
    // <ssid> <password> <hostIP>

    msg.getString(0, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
    config->setSsid((const char *)arg);

    if (msgLength == 2) { // no password
      config->setPassword("");
      msg.getString(1, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
      config->setHostIP((const char *)arg);
    } else { // msgLength >= 3, we have a password
      msg.getString(1, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
      config->setPassword((const char *)arg);
      msg.getString(2, (char *)arg, MAX_OSC_STRING_ARG_LENGTH);
      config->setHostIP((const char *)arg);
    }

    config->store();

    sendWiFiSettings(oscOutputSetWiFi);

    wifi->stopWiFi();
    wifi->startWiFi(); // will check if useWifi is set
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputGetWiFi]) == 0) {
    sendWiFiSettings(oscOutputGetWiFi);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputSetPorts]) == 0 && msgLength > 1) {
    int in = msg.getInt(0);
    int out = msg.getInt(1);

    in = in < 0 ? 0 : in;
    out = out < 0 ? 0 : out;

    config->setInputPort(in);
    config->setOutputPort(out);

    config->store();

    sendPorts(oscOutputSetPorts);
    
    wifi->stopWiFi();
    wifi->startWiFi(); // will check if useWifi is set
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputGetPorts]) == 0) {
    sendPorts(oscOutputGetPorts);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputSetRange]) == 0 && msgLength > 1) {
    sensors->setAccelRange(msg.getInt(0));
    sensors->setGyroRange(msg.getInt(1));

    config->setAccelRange(msg.getInt(0));
    config->setGyroRange(msg.getInt(1));

    config->store();
    sendAccelGyroRanges(oscOutputSetRange);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputGetRange]) == 0) {
    sendAccelGyroRanges(oscOutputGetRange);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputSetConfig]) == 0 && msgLength > 1) {
    sensors->setReadMagPeriod(msg.getInt(2));
    sensors->setOutputFramePeriod(msg.getInt(3));

    config->setUseSerial(msg.getInt(0) > 0);
    config->setSendSingleFrame(msg.getInt(1) > 0);
    config->setReadMagPeriod(msg.getInt(2));
    config->setOutputFramePeriod(msg.getInt(3));
    config->setButtonHoldDuration(msg.getInt(4));

    config->store();
    sendGlobalConfig(oscOutputSetConfig);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputGetConfig]) == 0) {
    sendGlobalConfig(oscOutputGetConfig);
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputSetAll]) == 0) {
  // TODO
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputGetAll]) == 0) {
  // TODO
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputVibroPulse]) == 0 && msgLength > 2) {
    vibrator->pulse(
      (unsigned long) msg.getInt(0),
      (unsigned long) msg.getInt(1),
      (unsigned long) msg.getInt(2)
    );
  //----------------------------------------------------------------------------
  } else if (strcmp(address, oscAddresses[oscInputVibroNow]) == 0 && msgLength > 0) {
    vibrator->vibrate(msg.getInt(0) != 0);
  //----------------------------------------------------------------------------
  }
}

void Router::sayHello() {
  OSCMessage msg(oscAddresses[oscOutputHello]);
  msg.add("movuino");
  msg.add(config->getMovuinoId());
  msg.add(config->getFirmwareVersion());
  msg.add(getConnectionStateOSCValue(wifi->getConnectionState()));
  msg.add(wifi->getStringIPAddress().c_str());

  sendSerialMessage(msg);
  msg.empty();
}

void
Router::sendWiFiConnectionMessage(int i) {
  OSCMessage msg(oscAddresses[oscOutputWiFiState]);
  msg.add(i);
  msg.add(wifi->getStringIPAddress().c_str());

  sendSerialMessage(msg);
  msg.empty();
}

void
Router::sendWiFiSettings(oscAddress a) {
  OSCMessage msg(oscAddresses[a]);
  msg.add(config->getSsid());
  msg.add(config->getPassword());
  msg.add(config->getHostIP());

  sendSerialMessage(msg);
  sendWiFiMessage(msg);
  msg.empty();
}

void
Router::sendPorts(oscAddress a) {
  OSCMessage msg(oscAddresses[a]);
  msg.add(config->getInputPort());
  msg.add(config->getOutputPort());

  sendSerialMessage(msg);
  sendWiFiMessage(msg);
  msg.empty();
}

void
Router::sendAccelGyroRanges(oscAddress a) {
  OSCMessage msg(oscAddresses[a]);
  msg.add(config->getAccelRange());
  msg.add(config->getGyroRange());

  sendSerialMessage(msg);
  sendWiFiMessage(msg);
  msg.empty();
}

void
Router::sendGlobalConfig(oscAddress a) {
  OSCMessage msg(oscAddresses[a]);
  msg.add(config->getUseSerial() ? "1" : "0");
  msg.add(config->getSendSingleFrame() ? "1" : "0");
  msg.add(config->getReadMagPeriod());
  msg.add(config->getOutputFramePeriod());
  msg.add(config->getButtonHoldDuration());

  sendSerialMessage(msg);
  sendWiFiMessage(msg);
  msg.empty();
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

  msg.empty();
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

  msg.empty();
}

void
Router::sendSingleFrame(float *f) {
  OSCMessage msg(oscAddresses[oscOutputFrame]);

  for (unsigned int i = 0; i < 9; ++i) {
    msg.add(*(f + i));
  }

  msg.add(getButtonIntValue(button->getState())); // append the button value
  msg.add(vibrator->isVibrating() ? 1 : 0);
  // sender ip already contained in every UDP packet, so no use for this :
  // msg.add(wifi->getStringIPAddress().c_str());

  if (config->getUseSerial()) {
    sendSerialMessage(msg);
  }

  if (config->getUseWiFi()) {
    sendWiFiMessage(msg);
  }

  msg.empty();
}

void
Router::sendWiFiMessage(OSCMessage& msg) {
  wifi->sendMessage(msg, config->getHostIP(), config->getOutputPort());
}

void
Router::sendSerialMessage(OSCMessage& msg) {
  serial->sendMessage(msg);
}
