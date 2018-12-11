
class c_led {
private:
  int pin;

public:
  void setup(int _pin) {
    pin = _pin;
    pinMode(pin, OUTPUT); // setting up the led pin to be an output
  }
  void set_led(bool state) {
    digitalWrite(pin, state); // writing to our led pin the state inputted
  };
