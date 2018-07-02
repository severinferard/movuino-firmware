#ifndef _MOVUINO_FIRMWARE_ACCESS_POINT_H_
#define _MOVUINO_FIRMWARE_ACCESS_POINT_H_

// TODO : put webserver code here

// use in wifiinterface ? or alone ?
// (we could stream data from accesspoint)

class Config;

class AccessPoint {
private:
  Config *config;

public:
  AccessPoint() {}
  ~AccessPoint() {}

  void init(Config *c);
  void update();
};

#endif /* _MOVUINO_FIRMWARE_ACCESS_POINT_H_ */
