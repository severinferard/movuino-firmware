#ifndef _MOVUINO_VIBRATOR_H_
#define _MOVUINO_VIBRATOR_H_

class Vibrator {
private:
  bool isPulsing;
  bool isConstantlyVibrating;
  bool isActuallyVibrating;

  unsigned long dVibOn, dVibOff, dVibTotal;
  float rVib;
  unsigned long vibTimer;
  long nVib;

public:
  Vibrator() :
  isPulsing(false), isConstantlyVibrating(false), isActuallyVibrating(false),
  dVibOn(0), dVibOff(0), dVibTotal(0), rVib(0), vibTimer(0), nVib(0) {}

  ~Vibrator() {}

  void update() {
    // swap to if (isConstantlyVibrating) else if (isPulsing) to give priority to constant vibration
    if (isPulsing) {
      unsigned long now = millis();
      // how many cycles we've already been through (floating point * 1000)
      int curTimeRatio1000 = (int) (1000 * (now - vibTimer) / (float) dVibTotal);

      if (curTimeRatio1000 % 1000 < (int) (1000 * rVib)) {
        isActuallyVibrating = true;
        digitalWrite(VIBRATOR_PIN, HIGH);
      } else {
        if (dVibOff != 0) {
          isActuallyVibrating = false;
          digitalWrite(VIBRATOR_PIN, LOW);
        }
      }

      // Shut down vibrator if number of cycles reach (set nVib to -1 for infinite cycles)
      if (nVib != -1 && (now - vibTimer > nVib * dVibTotal)) {
        isPulsing = isActuallyVibrating = false;
        digitalWrite(VIBRATOR_PIN, LOW);
      }
    } else if (isConstantlyVibrating) {
      isActuallyVibrating = true;
      digitalWrite(VIBRATOR_PIN, HIGH);
    } else {
      isActuallyVibrating = false;
      digitalWrite(VIBRATOR_PIN, LOW);
    }
  }

  void pulse(unsigned long onDuration, unsigned long offDuration, long nb) {
    dVibOn = onDuration;
    dVibOff = offDuration;
    nVib = nb;

    if (dVibOn == 0) {
      isPulsing = false;
    } else {
      dVibTotal = dVibOn + dVibOff;
      rVib = dVibOn / (float) dVibTotal;
      vibTimer = millis();
      isPulsing = true;
    }
  }

  void vibrate(bool vibOnOff) {
    if (vibOnOff) {
      isConstantlyVibrating = true;
    } else {
      isConstantlyVibrating = false;
    }
  }

  bool isVibrating() {
    return isActuallyVibrating;
  }
};

#endif /* _MOVUINO_VIBRATOR_H_ */
