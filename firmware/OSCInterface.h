#ifndef _MOVUINO_OSC_INTERFACE_H_
#define _MOVUINO_OSC_INTERFACE_H_

#include <OSCMessage.h>
#include <ESPEventEmitter.h>

////////// base class for SerialInterface and WiFiInterface :

class OSCInterface {
protected:
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  class OSCMessageEventEmitter : public ESPEventEmitter<OSCMessage&> {
  private:
    char address[128]; // maximum osc address length
    
  public:
    OSCMessageEventEmitter() : ESPEventEmitter<OSCMessage&>() {}
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

  void addOSCMessageListener(const char *address, std::function<void(OSCMessage&)> callback) {
    oscEmitter.addListener(address, callback);    
  }

  void removeOSCMessageListener(const char *address) {
    oscEmitter.removeListeners(address);
  }

  virtual void sendOSCMessage(OSCMessage& msg) = 0; // must be defined in child classes
};

#endif /* _MOVUINO_OSC_INTERFACE_H_ */