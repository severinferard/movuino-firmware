#ifndef _MOVUINO_INTEGRATED_SENSORS_H_
#define _MOVUINO_INTEGRATED_SENSORS_H_

#include <MPU9250_asukiaaa.h>

class IntegratedSensors {
private:
  MPU9250 *mpu;

public:
  IntegratedSensors() {
    // mpu = new MPU9250(MPU9250_ADDRESS_AD0_LOW); // default (see MPU9250_asukiaaa.h)
    mpu = new MPU9250(MPU9250_ADDRESS_AD0_HIGH); // this is the right one for movuino esp32
  }

  ~IntegratedSensors() {
    delete mpu;
  }

  void init() {
    Wire.begin();
    mpu->setWire(&Wire);
  }

  void update();

  float getAccelX();
  float getAccelY();
  float getAccelZ();

  float getGyroX();
  float getGyroY();
  float getGyroZ();

  float getMagX();
  float getMagY();
  float getMagZ();

  void enableMag(bool e);

private:
  void magAutoCalibration();
};

#endif /* _MOVUINO_INTEGRATED_SENSORS_H_ */