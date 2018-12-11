#include <Servo.h>

class c_servos {
private:
  int right_pin;
  int left_pin;
  int right_stop_value;
  int left_stop_value;
  Servo right_wheel;
  Servo left_wheel;
  float time_taken_one_rotation_ms;
  float time_taken_travel_30cm;

public:
  void setup(int right_pin_, int left_pin_, int right_stop_val,
             int left_stop_val, float time_for_full_rotation_ms,
             float time_taken_to_travel_30cm) {
    right_pin = right_pin_;
    left_pin = left_pin_;
    right_stop_value = right_stop_val;
    left_stop_value = left_stop_val;
    time_taken_one_rotation_ms = time_for_full_rotation_ms;
    time_taken_travel_30cm = time_taken_to_travel_30cm;

    // attaching our servos
    right_wheel.attach(right_pin);
    left_wheel.attach(left_pin);
  }
  void set_speed(int right_speed, int left_speed) { // sets wheel speeds
    // servos are setup differently so right needs to decrease left increase to
    // go forwards.
    right_wheel.write(right_stop_value - right_speed);
    left_wheel.write(left_stop_value + left_speed);
  }
  void turn(float deg, bool right) { // turns in direction of the bool (true =
                                     // clockwise, false = anticlockwise)
    /*
      simple equation to convert the degrees into how long we need
      to turn in miliseconds to turn that amount of degrees
      using time it takes at set speed to do one whole
      rotation dividing by 360(one whole rotation) and then
      multiplying by desired degrees for the turn
    */
    auto time_to_delay = deg * (time_taken_one_rotation_ms / 360);

    if (right) { // bool input so we decide if we go clockwise or anticlockwise
      right_wheel.write(right_stop_value + 10);
      left_wheel.write(left_stop_value - 10);
      delay(time_to_delay); // how long to keep turning for...
      stop();               // stop motors
    } else {
      right_wheel.write(right_stop_value + 10);
      left_wheel.write(left_stop_value - 10);
      delay(time_to_delay); // how long to keep turning for...
      stop();               // stop motors
    }
  }
  void forward(int right_speed, int left_speed,
               float dist_cm) { // taking input of desired distance in cm's

    // using time it takes at set speed to travel exactly 30cms
    // dividing it by 30 so we know the time it takes for 1cm travel
    // and then multiplying it by our desired distance in cm
    auto time_to_delay = dist_cm * (time_taken_travel_30cm / 30);
    set_speed(right_speed, left_speed);
    delay(time_to_delay); // go forward till delay is over..
    stop();               // stop going forward
  }
  void stop() { // set motors to standstill
    right_wheel.write(right_stop_value);
    left_wheel.write(left_stop_value);
  }
};
