#ifndef _MOVUINO_TIMER_H_
#define _MOVUINO_TIMER_H_

#include <EventEmitter.h>

template <typename ...T>
class Timer : public EventEmitter<...T> {
private:
  unsigned long period;
  unsigned long lastTickDate;

public:
  Timer(unsigned long p) : EventEmitter<unsigned long>(),
  period(p), lastTickDate(0) {}

  ~Timer() {}

  void setPeriod(unsigned long p) { period = p; }

  void update(T... t) {
    unsigned long now = millis();
    
    if (now - lastTickDate >= period) {
      lastTickDate = now;
      emit("tick", t);
    }
  }
};

#endif /* _MOVUINO_TIMER_H_ */
