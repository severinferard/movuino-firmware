#include "Config.h"
#include "Router.h"
#include "SerialInterface.h"

void
SerialInterface::init(Config *c, Router *r) {
  config = c;
  router = r;

  slip->begin(115200);
}

void
SerialInterface::update() {
  readMessages();
}

void
SerialInterface::readMessages(/*Router *router*/) {
  if (slip->available() > 0) {
    int size;

    while (!slip->endofPacket()) {
      if ((size = slip->available()) > 0) {
        while (size--) {
          inputOSCMessage.fill(slip->read());
        }
      }
    }

    if (!inputOSCMessage.hasError()) {
      router->routeSerialMessage(inputOSCMessage);        
    }

    inputOSCMessage.empty();
  }
}

bool
SerialInterface::sendMessage(OSCMessage& msg) {
  slip->beginPacket();
  msg.send(*slip);
  slip->endPacket();
}
