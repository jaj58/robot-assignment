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
    void setup(int right_pin_, int left_pin_, int right_stop_val, int left_stop_val, float time_for_full_rotation_ms, float time_taken_to_travel_30cm) { //pin for servo, 0 movement value for servo, time it takes for one full rotation in miliseconds
      right_pin = right_pin_;
      left_pin = left_pin_;
      right_stop_value = right_stop_val;
      left_stop_value = left_stop_val;
      time_taken_one_rotation_ms = time_for_full_rotation_ms;
      time_taken_travel_30cm = time_taken_to_travel_30cm;
      right_wheel.attach(right_pin);
      left_wheel.attach(left_pin);
    }
    void set_speed(int right_speed, int left_speed) { //sets wheel speeds
      right_wheel.write(right_stop_value - right_speed);
      left_wheel.write(left_stop_value + left_speed);
    }
    void turn(float deg, bool right) {//turns in direction of the bool (true = clockwise, false = anticlockwise)
      /*
      	simple equation to convert the degrees into how long we need
      	to turn in miliseconds to turn that amount of degrees
      */
      auto time_to_delay = deg * (time_taken_one_rotation_ms / 360);

      if (right) {
        right_wheel.write(right_stop_value + 5);
        left_wheel.write(left_stop_value - 5);
        delay(time_to_delay);
        stop();
      }
      else {
        right_wheel.write(right_stop_value + 5);
        left_wheel.write(left_stop_value - 5);
        delay(time_to_delay);
        stop();
      }
    }
    void forward(int right_speed, int left_speed, float dist_cm) { //taking input of desired distance in cm's{
      auto time_to_delay = dist_cm * (time_taken_travel_30cm / 30);
      set_speed(right_speed, left_speed);
      delay(time_to_delay);
    }
    void stop() {
      right_wheel.write(right_stop_value);
      left_wheel.write(left_stop_value);
    }

    /*

      commented out since not needed atm...
      //getters and setters
      void set_pin(int _pin) //change the pin of the servo
    	pin = _pin;
      }
      int get_pin(){//returns current pin of the servo
    	return pin;
      }
      void set_stop_value(int x){ //changes stop value of the servo
    	stop_value = x;
      }
      int get_stop_value(){ //returns current stop value of the servo
    	return stop_value;
      }
    */
};
