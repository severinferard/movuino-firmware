#ifndef _MOVUINO_OSC_INTERFACE_H_
#define _MOVUINO_OSC_INTERFACE_H_

#include <EventEmitter.h>
#include <OSCMessage.h>

////////// base class for serial and wifi :

class OSCInterface {
protected:
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  class OSCMessageEventEmitter : public EventEmitter<OSCMessage&> {
  private:
    char address[128]; // maximum osc address length
    
  public:
    OSCMessageEventEmitter() : EventEmitter<OSCMessage&>() {}
    ~OSCMessageEventEmitter() {}

    void emitOSCMessage(OSCMessage& msg) {
      msg.getAddress(address);
      emit(address, msg);
    }
  };
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  OSCMessageEventEmitter oscEmitter;

public:
  OSCInterface() {}
  virtual ~OSCInterface() {}

  void addOSCMessageListener(const char *address, void (*callback)(OSCMessage& msg)) {
    oscEmitter.addListener(event, callback);    
  }

  void removeOSCMessageListener(const char *address) {
    oscEmitter.removeListeners(address);
  }

  void sendOSCMessage(OSCMessage& msg) = 0; // must be defined in child classes
};

#endif /* _MOVUINO_OSC_INTERFACE_H_ */