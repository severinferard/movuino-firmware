#ifndef _MOVUINO_INTEGRATED_SENSORS_H_
#define _MOVUINO_INTEGRATED_SENSORS_H_

#include <MPU9250_asukiaaa.h>

class IntegratedSensors {
private:
  MPU9250_asukiaaa *mpu;

  int accelRange;
  float accelDivider;
  
  int gyroRange;
  float gyroDivider;
  
  bool readMag;

  float fmx, fmy, fmz;
  float magRange[6];
  float values[9];

public:
  IntegratedSensors() :
  accelRange(0), accelDivider(1),
  gyroRange(0), gyroDivider(1),
  readMag(false) {
    // mpu = new MPU9250_asukiaaa(MPU9250_ADDRESS_AD0_LOW); // default (see MPU9250_asukiaaa.h)
    mpu = new MPU9250_asukiaaa(MPU9250_ADDRESS_AD0_HIGH); // this is the right one for movuino esp32
  }

  ~IntegratedSensors() {
    delete mpu;
  }

  void init() {
#ifdef ESP8266
    Wire.begin();
#elif defined(ESP32)
    Wire.begin(SDA_PIN, SCL_PIN);
#endif
    
    mpu->setWire(&Wire);
    mpu->beginMag(MAG_MODE_CONTINUOUS_100HZ);

    for (unsigned int i = 0; i < 3; ++i) {
      magRange[i * 2] = 666;
      magRange[i * 2 + 1] = -666;
    }
  }

  void update() {
    mpu->accelUpdate();
    mpu->gyroUpdate();

    values[0] = mpu->accelX() / accelDivider;
    values[1] = mpu->accelY() / accelDivider;
    values[2] = mpu->accelZ() / accelDivider;

    values[3] = mpu->gyroX() / gyroDivider;
    values[4] = mpu->gyroY() / gyroDivider;
    values[5] = mpu->gyroZ() / gyroDivider;

    if (readMag) {
      mpu->magUpdate();

      fmx = mpu->magX();
      fmy = mpu->magY();
      fmz = mpu->magZ();

      magAutoCalibration();

      values[6] = fmx;
      values[7] = fmy;
      values[8] = fmz;
    } else {
      values[6] = 0;
      values[7] = 0;
      values[8] = 0;
    }
  }

  void setAccelRange(int r) {
    int rr = r > 3 ? 3 : (r < 0 ? 0 : r);
    
    switch (rr) {
      case 0:
        mpu->beginAccel(ACC_FULL_SCALE_2_G);
        accelDivider = 1; // 2;
        break;
      case 1:
        mpu->beginAccel(ACC_FULL_SCALE_4_G);
        accelDivider = 2; // 4;
        break;
      case 2:
        mpu->beginAccel(ACC_FULL_SCALE_8_G);
        accelDivider = 4; // 8;
        break;
      case 3:
        mpu->beginAccel(ACC_FULL_SCALE_16_G);
        accelDivider = 8; // 16;
        break;
      default:
        return;
    }
  }

  void setGyroRange(int r) {
    int rr = r > 3 ? 3 : (r < 0 ? 0 : r);

    switch (rr) {
      case 0:
        mpu->beginGyro(GYRO_FULL_SCALE_250_DPS);
        gyroDivider = 125; // 250;
        break;
      case 1:
        mpu->beginGyro(GYRO_FULL_SCALE_500_DPS);
        gyroDivider = 250; // 500;
        break;
      case 2:
        mpu->beginGyro(GYRO_FULL_SCALE_1000_DPS);
        gyroDivider = 500; // 1000;
        break;
      case 3:
        mpu->beginGyro(GYRO_FULL_SCALE_2000_DPS);
        gyroDivider = 1000; // 2000;
        break;
      default:
        return;
    }
  }

  void enableMag(bool e) {
    readMag = e;

    // this is buggy :
    
    if (readMag) {
      // mpu->magSetMode(MAG_MODE_POWERDOWN);
    } else {
      // mpu->beginMag(MAG_MODE_CONTINUOUS_100HZ);
    }
  }

  float getAccelX() { return values[0]; }
  float getAccelY() { return values[1]; }
  float getAccelZ() { return values[2]; }

  float getGyroX() { return values[3]; }
  float getGyroY() { return values[4]; }
  float getGyroZ() { return values[5]; }

  float getMagX() { return values[6]; }
  float getMagY() { return values[7]; }
  float getMagZ() { return values[8]; }

private:
  // TODO : try asukiaaa's algorithms
  void magAutoCalibration() {
    float magVal[] = { fmx, fmy, fmz };

    for (int i = 0; i < 3; i++) {
      // Compute magnetometer range
      if (magVal[i] < magRange[2 * i]) {
        magRange[2 * i] = magVal[i]; // update minimum values on each axis
      }

      if (magVal[i] > magRange[2 * i + 1]) {
        magRange[2 * i + 1] = magVal[i]; // update maximum values on each axis
      }

      // Scale magnetometer values
      if (magRange[2*i] != magRange[2*i+1]) {
        // magVal[i] = map(magVal[i], magRange[2*i], magRange[2*i+1], -100, 100);
        magVal[i] = (magVal[i] - magRange[2 * i]) / (magRange[2 * i + 1] - magRange[2 * i]);
        magVal[i] *= 2;
        magVal[i] -= 1;
      }
    }

    // Update magnetometer values
    fmx = magVal[0];
    fmy = magVal[1];
    fmz = magVal[2];
  }
};

#endif /* _MOVUINO_INTEGRATED_SENSORS_H_ */
