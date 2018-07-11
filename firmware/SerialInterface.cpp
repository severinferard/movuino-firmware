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
    OSCMessage msg;
    int size;

    while (!slip->endofPacket()) {
      if ((size = slip->available()) > 0) {
        while (size--) {
          msg.fill(slip->read());
        }
      }
    }

    if (!msg.hasError()) {
      router->routeSerialMessage(msg);        
    }
  }
}

bool
SerialInterface::sendMessage(OSCMessage& msg) {
  slip->beginPacket();
  msg.send(*slip);
  slip->endPacket();
}
