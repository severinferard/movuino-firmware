#ifndef _MOVUINO_BUTTON_H_
#define _MOVUINO_BUTTON_H_

class Button {
private:
  bool btnOn;
  bool btnHolding;
  unsigned long lastBtnDate;
  ButtonState state;

public:
  Button() :
  btnOn(false), btnHolding(false), lastBtnDate(0), state(ButtonStateNone) {}

  ~Button() {}

  void update() {
    bool btn = digitalRead(BUTTON_PIN) == 0;
    unsigned long now = millis();

    if (btn && !btnOn) {
      btnOn = btnHolding = true;
      lastBtnDate = now;
      state = ButtonStatePressed;
    } else if (!btn && btnOn) {
      btnOn = btnHolding = false;
      state = ButtonStateReleased;
    } else if (btnOn && btnHolding &&
               now - lastBtnDate > BUTTON_STATE_HOLD_DURATION) {
      btnHolding = false;
      state = ButtonStateHolding;
    }
  }

  ButtonState getState() { return state; }
};

#endif /* _MOVUINO_BUTTON_H_ */
