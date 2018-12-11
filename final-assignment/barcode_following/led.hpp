
class c_led {
  private:
    int pin;
  public:
    void setup(int _pin) {
      pin = _pin;
      pinMode(pin, OUTPUT);
    }
    void set_led(bool state) {
      digitalWrite(pin, state);
    }
};
