#ifndef _MOVUINO_SERIAL_INTERFACE_H_
#define _MOVUINO_SERIAL_INTERFACE_H_

#include <Arduino.h>
#include <SLIPEncodedSerial.h>
#include <SLIPEncodedUSBSerial.h>
#include "OSCInterface.h"

class SerialInterface : public OSCInterface {
private:
  SLIPEncodedSerial *slip;
  OSCMessage inputOSCMessage;

public:
  SerialInterface() : OSCInterface() {
    slip = new SLIPEncodedSerial(Serial);
  }
  
  ~SerialInterface() {
    delete slip;
  }

  void start() {
    slip->begin(115200);
  }

  void update() {
    if (slip->available() > 0) {
      int size;
      inputOSCMessage.empty();

      while (!slip->endofPacket()) {
        if ((size = slip->available()) > 0) {
          while (size--) {
            inputOSCMessage.fill(slip->read());
          }
        }
      }

      if (!inputOSCMessage.hasError()) {
        oscEmitter.emitOSCMessage(inputOSCMessage);
      }
    }    
  }

  void sendOSCMessage(OSCMessage& msg) {
    slip->beginPacket();
    msg.send(*slip);
    slip->endPacket();
  }
};

#endif /* _MOVUINO_SERIAL_INTERFACE_H_ */