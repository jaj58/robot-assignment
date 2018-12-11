
class c_ldr {
  private:
    int pin;
    int lowest_light_val, highest_light_val, lowest_dark_val, highest_dark_val;
  public:
    void setup(int _pin) {
      pin = _pin;
      pinMode(pin, INPUT);
    }
    int read() {
      return analogRead(pin);
    }
    void calibrate(bool light) { //false for dark calibration
      for (auto i = 0; i < 10; i++) {
        int temp_val = read();
        if (light) {
          if (temp_val > highest_light_val) {
            highest_light_val = temp_val;
            if (lowest_light_val == 0 ) {
              lowest_light_val = highest_light_val - 100;
            }
          }
          else if (temp_val < lowest_light_val || lowest_light_val == 0) {
            lowest_light_val = temp_val;
          }
        }
        else {
          if (temp_val > highest_dark_val) {
            highest_dark_val = temp_val;
            if (lowest_dark_val == 0) {
              lowest_dark_val = highest_dark_val - 100;
            }
          }
          else if (temp_val < lowest_dark_val || lowest_light_val == 0) {
            lowest_dark_val = temp_val;
          }
        }
        delay(100);
      }
      debug_print("finished calibrating ldr");
    }
    bool is_light() { //check to see if ldr sees light(true) or dark(false)
      auto val = read();
      delay(20);
      auto val1 = read();
      delay(20);
      auto val2 = read();
      delay(20);
      auto val3 = read();
      delay(20);
      auto val4 = read();
      auto average_val = ( (val + val1 + val2 + val3 + val4) / 5);
      auto dif = (lowest_light_val - highest_dark_val) / 2;
      if ((lowest_light_val - dif) < average_val && average_val < (highest_light_val + dif + 300)) {
        //debug_print("ldr detected light");
        return true;
      }
      else if (0 < average_val && average_val < (highest_dark_val + dif)) { //(lowest_dark_val - dif)
        //debug_print("ldr detected dark");
        return false;
      }
      else if (average_val == (highest_dark_val + dif)) {
        return false;
      }
      else if (average_val == (lowest_light_val - dif)) {
        return true;
      }
      else {
        /*
          this should probably be changed to decide what the read value is closer
          to or if we should read again to see if there was an error/ inconsistency
        */
        //debug_print("Invalid value, Should re-calibrate lowest and highest values.");
      }
    }
    //getters
    int get_lowest_light_value() {
      return lowest_light_val;
    }
    int get_lowest_dark_value() {
      return lowest_dark_val;
    }
    int get_highest_light_value() {
      return highest_light_val;
    }
    int get_highest_dark_value() {
      return highest_dark_val;
    }
    //setters
    void set_lowest_light_value(int val) {
      lowest_light_val = val;
    }
    void set_lowest_dark_value(int val) {
      lowest_dark_val = val;
    }
    void set_highest_light_value(int val) {
      highest_light_val = val;
    }
    void set_highest_dark_value(int val) {
      highest_dark_val = val;
    }
};
