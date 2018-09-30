#include <Arduino.h>
#include "I2Cdev.h"
#include "Config.h"
#include "Router.h"
#include "Sensors.h"

//================================= TIMER ====================================//

/**
 * adapted to be non blocking using a state flag
 * from the original sparkun / Jeff Rowberg MPU6050 library:
 * MPU6050::getMag(int16_t *mx, int16_t *my, int16_t *mz)
 * there used to be delay(10)'s between calls to writeByte and readBytes,
 * this function is now called in a non-blocking loop (a period of 10 ms works fine)
 * and keeps its own state up to date.
 */
void
MagTimer::callback() {
  if (readMagState == 0) {
    // set i2c bypass enable pin to true to access magnetometer
    I2Cdev::writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x02);
  } else if (readMagState == 1) {
    // enable the magnetometer
    I2Cdev::writeByte(MPU9150_RA_MAG_ADDRESS, 0x0A, 0x01);
  } else {
    // read it !
    I2Cdev::readBytes(MPU9150_RA_MAG_ADDRESS, MPU9150_RA_MAG_XOUT_L, 6, magBuffer);
    uint16_t mx = (((int16_t)magBuffer[1]) << 8) | magBuffer[0];
    uint16_t my = (((int16_t)magBuffer[3]) << 8) | magBuffer[2];
    uint16_t mz = (((int16_t)magBuffer[5]) << 8) | magBuffer[4];

    sensors->setRawMagValues(mx, my, mz);

    // reset i2c bypass enable pin as soon as we are done
    // therefore it take a total of (2 * timer period) to read new magnetometer values
    // callback();
    // or if we don't call callback here, a total of (3 * timer period)
  }

  readMagState = (readMagState + 1) % 3;
}

void
OSCOutTimer::callback() {
  sensors->sendSensorValues();
}

//================================ SENSORS ===================================//

void
Sensors::init(Config *c, Router *r) {
  config = c;
  router = r;

  mpu.initialize();
  setAccelRange(config->getAccelRange());
  setGyroRange(config->getGyroRange());
  setReadMag(config->getReadMag());
  
  for (unsigned int i = 0; i < 3; ++i) {
    magRange[i * 2] = 666;
    magRange[i * 2 + 1] = -666;
  }

  if (MOVUINO_READ_MAG_ASYNC) {
    // readMagTimer->setPeriod(config->getReadMagPeriod());
    readMagTimer->setPeriod(DEFAULT_READ_MAG_PERIOD);
    oscOutTimer->setPeriod(config->getOutputFramePeriod());
    
    readMagTimer->start();
    oscOutTimer->start();
  }
}

void
Sensors::update() {
  if (MOVUINO_READ_MAG_ASYNC) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    updateAccelGyroValues();
    
    if (config->getReadMag()) {
      readMagTimer->update();
    }
    
    oscOutTimer->update();
  } else {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    updateAccelGyroValues();

    if (config->getReadMag()) {
      readMagValues();
      updateMagValues();
    }

    sendSensorValues();
  }
}

////////// SENSOR RANGES

int
Sensors::getAccelRange() {
  return mpu.getFullScaleAccelRange();
}

void
Sensors::setAccelRange(int r) {
  mpu.setFullScaleAccelRange(r);
}

int
Sensors::getGyroRange() {
  return mpu.getFullScaleGyroRange();
}

void
Sensors::setGyroRange(int r) {
  mpu.setFullScaleGyroRange(r);
}

void
Sensors::setReadMag(bool b) {
  readMag = b;

  if (!readMag) {
    mx = my = mz = 0;
  }
}

void
Sensors::setReadMagPeriod(int p) {
  readMagTimer->setPeriod(p);
}

void
Sensors::setOutputFramePeriod(int p) {
  oscOutTimer->setPeriod(p);
}

//-------------------------------- PRIVATE -----------------------------------//

// original readMag method, to allow  synchronous reading
// (Timers must be off and sendSensorValues must be called explicitly)
//*
void
Sensors::readMagValues() {
  // set i2c bypass enable pin to true to access magnetometer
  I2Cdev::writeByte(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x02);
  delay(10);
  // enable the magnetometer
  I2Cdev::writeByte(MPU9150_RA_MAG_ADDRESS, 0x0A, 0x01);
  delay(10);
  // read it !
  I2Cdev::readBytes(MPU9150_RA_MAG_ADDRESS, MPU9150_RA_MAG_XOUT_L, 6, magBuffer);
  mx = (((int16_t)magBuffer[1]) << 8) | magBuffer[0];
  my = (((int16_t)magBuffer[3]) << 8) | magBuffer[2];
  mz = (((int16_t)magBuffer[5]) << 8) | magBuffer[4];

  updateMagValues();
}
//*/

////////// MagTimer callback :
void
Sensors::setRawMagValues(uint16_t x, uint16_t y, uint16_t z) {
  mx = x;
  my = y;
  mz = z;

  // update only on real data change
  updateMagValues();
}

////////// OSCOutTimer callback :
void
Sensors::sendSensorValues() {
  router->onNewSensorValues(&(values[0]));
}

void
Sensors::updateAccelGyroValues() {
  values[0] = ax / float(32768);
  values[1] = ay / float(32768);
  values[2] = az / float(32768);

  values[3] = gx / float(32768);
  values[4] = gy / float(32768);
  values[5] = gz / float(32768);
}

void
Sensors::updateMagValues() {
  magnetometerAutoCalibration();
  // or use some future calibration procedure result

  values[6] = mx / float(100);
  values[7] = my / float(100);
  values[8] = mz / float(100);
}

void
Sensors::magnetometerAutoCalibration() {
  int magVal[] = { mx, my, mz };

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
      magVal[i] = map(magVal[i], magRange[2*i], magRange[2*i+1], -100, 100);
    }
  }

  // Update magnetometer values
  mx = magVal[0];
  my = magVal[1];
  mz = magVal[2];
}

