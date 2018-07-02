#ifndef _MOVUINO_FIRMWARE_SERIALINTERFACE_H_
#define _MOVUINO_FIRMWARE_SERIALINTERFACE_H_

#include <Arduino.h>
#include <SLIPEncodedSerial.h>
#include <SLIPEncodedUSBSerial.h>
#include <OSCMessage.h>

class Router;

class SerialInterface {
private:
  SLIPEncodedSerial *slip;

  Config *config;
  Router *router;

public:
  SerialInterface() {
    slip = new SLIPEncodedSerial(Serial);
  }

  ~SerialInterface() {
    delete slip;
  }

  void init(Config *c, Router *r);
  void update();

  void readMessages(/*Router *router*/);
  bool sendMessage(OSCMessage& msg);
};

#endif /* _MOVUINO_FIRMWARE_SERIALINTERFACE_H_ */
