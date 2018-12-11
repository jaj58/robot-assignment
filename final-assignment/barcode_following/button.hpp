
class c_button {
  private:
    int pin;

  public:
    void setup(int _pin) {
      pin = _pin;
      pinMode(pin, INPUT);
    }
    void wait() { //waits for button press...
      debug_print("waiting for button press...");
      while (digitalRead(pin) == HIGH) {
      }
      delay(50);
      debug_print("button pressed, waiting for release...");
      while (digitalRead(pin) == LOW) {

      }
      debug_print("button released, returning...");
      delay(50);
      return;
    }
};
