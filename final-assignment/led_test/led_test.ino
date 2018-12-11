#define DEBUG

#ifdef DEBUG
void debug_print(String msg) { //just a simple printing method that only works when DEBUG is defined in the program.
  String new_msg = String("[DEBUG]: ");
  new_msg = new_msg + msg;
  Serial.println(new_msg);
}
#endif

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
      String dbg_msg = String("led state: ");
      dbg_msg = dbg_msg + state;
      debug_print(dbg_msg); //double check code.
    }
};

c_led red_led;
c_led yellow_led;
c_led green_led;

const short red_led_pin = 13;
const short yellow_led_pin = 12;
const short green_led_pin = 7;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  red_led.setup(red_led_pin);
  yellow_led.setup(yellow_led_pin);
  green_led.setup(green_led_pin);
}

void loop() {
  // put your main code here, to run repeatedly:
  red_led.set_led(true);
  delay(500);
  yellow_led.set_led(true);
  delay(500);
  green_led.set_led(true);
  delay(500);
  red_led.set_led(false);
  delay(500);
  yellow_led.set_led(false);
  delay(500);
  green_led.set_led(false);
  delay(500);
}
