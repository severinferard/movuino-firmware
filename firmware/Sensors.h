#ifndef _MOVUINO_FIRMWARE_SENSORS_H_
#define _MOVUINO_FIRMWARE_SENSORS_H_

#include "MPU6050.h"
#include "Timer.h"

class Config;
class Router;
class Sensors;

//----------------------------------------------------------------------------//

// child class of Timer specialized in reading magnetometer sensor values

class MagTimer : public Timer {
private:
  Sensors *sensors;
  unsigned int readMagState;
  uint8_t magBuffer[14]; // used for reading values from magnetometer

public:
  MagTimer(Sensors *s, unsigned long period) :
  Timer(period), sensors(s), readMagState(0) {}

  virtual ~MagTimer() {}

  // reads values, then calls sensors->setRawMagValues()
  virtual void callback();
};

//----------------------------------------------------------------------------//

class OSCOutTimer : public Timer {
private:
  Sensors *sensors;

public:
  OSCOutTimer(Sensors *s, unsigned long period) :
  Timer(period), sensors(s) {}

  virtual ~OSCOutTimer() {}

  // tell the sensors class to send a frame to the router
  virtual void callback();
};

//=========================== ACTUAL SENSORS CLASS ===========================//

class Sensors {
  friend class MagTimer;
  friend class OSCOutTimer;

private:
  // composition over inheritance !
  MagTimer *readMagTimer;
  OSCOutTimer *oscOutTimer;

  MPU6050 mpu;

  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  int16_t mx, my, mz;
  float values[9];
  int magRange[6];// = {666, -666, 666, -666, 666, -666}; // magneto range values for calibration
  uint8_t magBuffer[14]; // for synchronous reading


  Config *config;
  Router *router;

public:
  Sensors() {
    readMagTimer = new MagTimer(this, 10);
    oscOutTimer = new OSCOutTimer(this, 10);
  }

  ~Sensors() {
    delete readMagTimer;
    delete oscOutTimer;
  }

  void init(Config *c, Router *r); // initialize device, set ranges and start timer
  void update(); // accel and gyro values are read on each call to update()

  int getAccelRange();
  void setAccelRange(int r);

  int getGyroRange();
  void setGyroRange(int r);

  void setReadMagPeriod(int p);
  void setOutputFramePeriod(int p);

private:
  // use either synchronous read with this method, or MagTimer
  // void readMagValues(); // => getMotion9 is used instead

  // only executed by friend class MagTimer
  void setRawMagValues(uint16_t x, uint16_t y, uint16_t z);
  // only executed by friend class OSCOutTimer
  void sendSensorValues();

  void updateAccelGyroValues();
  void updateMagValues(); // todo : re-enable auto calibration
  void magnetometerAutoCalibration();
};

#endif /* _MOVUINO_FIRMWARE_SENSORS_H_ */
