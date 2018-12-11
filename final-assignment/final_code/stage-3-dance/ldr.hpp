
class c_ldr {
private:
  int pin;
  int lowest_light_val, highest_light_val, lowest_dark_val, highest_dark_val;

public:
  void setup(int _pin) {
    pin = _pin;
    pinMode(pin, INPUT); // setting the pin to take input
  }
  int read() {
    return analogRead(pin); // reading from the pin defined earlier
  }
  void calibrate(bool light) { // false for dark calibration true for light
    for (auto i = 0; i < 10; i++) {
      // loop through this 10 times to get a better average...
      int temp_val = read();
      if (light) {
        if (temp_val > highest_light_val) { // value we just read is greater
                                            // than previous highest value
          highest_light_val = temp_val;
          // since our new value is now the highest light val...
          if (lowest_light_val == 0) {
            // if we still dont have a high light val set one -100 from
            // the highest just incase...
            lowest_light_val = highest_light_val - 100;
          }
        } else if (temp_val < lowest_light_val || lowest_light_val == 0) {
          // the temp val is lower than the previous lowest
          // val or it is still 0...
          lowest_light_val = temp_val; // set new value to lowest light val
        }
      } else { // same calibration method as for light ....
        if (temp_val > highest_dark_val) {
          // temp val is grater than current highest value
          highest_dark_val = temp_val;
          if (lowest_dark_val == 0) {
            // lowest dark value hasnt been set at all yet (fallback...)
            lowest_dark_val = highest_dark_val - 100;
          }
        } else if (temp_val < lowest_dark_val || lowest_dark_val == 0) {
          // read value is less than current
          // lowest value or is 0 still...
          lowest_dark_val = temp_val;
        }
      }
      delay(100);
    }
    debug_print("finished calibrating ldr");
  }
  bool is_light() { // check to see if ldr sees light(true) or dark(false)
    // taking 5 readings in total with gaps of 20 ms delay to try gain a better
    // average value..
    auto val = read();
    delay(20);
    auto val1 = read();
    delay(20);
    auto val2 = read();
    delay(20);
    auto val3 = read();
    delay(20);
    auto val4 = read();
    // adding all our read values and dividing by 5 to gain a average
    auto average_val = ((val + val1 + val2 + val3 + val4) / 5);
    // calculating the difference between lowest light value and highest
    // dark value so we can fill in any gaps in between for our range
    auto dif = (lowest_light_val - highest_dark_val) / 2;
    if ((lowest_light_val - dif) < average_val &&
        average_val < (highest_light_val + dif)) {
      // if the value read is between our range for light
      // values
      return true;
    } else if ((lowest_dark_val - dif) < average_val &&
               average_val < (highest_dark_val + dif)) {
      // if our value is between our range for dark
      // values
      return false;
    } else if (average_val == (highest_dark_val + dif)) {
      return false;
    } else if (average_val == (lowest_light_val - dif)) {
      return true;
    } else {
      /*
      add a fallback if it reaches here too often then we need to re callibrate
      and decide whats going wrong....
      */
    }
  }
  // getters
  int get_lowest_light_value() { return lowest_light_val; }
  int get_lowest_dark_value() { return lowest_dark_val; }
  int get_highest_light_value() { return highest_light_val; }
  int get_highest_dark_value() { return highest_dark_val; }
  // setters
  void set_lowest_light_value(int val) { lowest_light_val = val; }
  void set_lowest_dark_value(int val) { lowest_dark_val = val; }
  void set_highest_light_value(int val) { highest_light_val = val; }
  void set_highest_dark_value(int val) { highest_dark_val = val; }
};
