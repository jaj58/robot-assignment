#include <Servo.h>
#include "debug.hpp"

class c_servos{
private:
int right_pin;
int left_pin
int right_stop_value;
int left_stop_value;
Servo right_wheel;
Servo left_wheel;
float time_taken_one_rotation_ms;
float time_taken_travel_30cm;
public:
void setup(int right_pin_, int left_pin_, int right_stop_val, int left_stop_val, float time_for_full_rotation_ms, float time_taken_to_travel_30cm){ //pin for servo, 0 movement value for servo, time it takes for one full rotation in miliseconds
	right_pin = right_pin_;
	left_pin = left_pin_;
	right_stop_value = right_stop_val;
	left_stop_value = left_stop_val;
	time_taken_one_rotation_ms = time_for_full_rotation_ms;
	time_taken_to_travel_30cm = time_taken_travel_30cm;
	right_wheel.attach(right_pin);
	left_wheel.attaach(left_pin);
	if(right_wheel.attached()){
		debug_print(String("wheel at pin " + right_pin + " is attached."));
	}
	else{
		debug_print(String("wheel at pin " + right_pin + " didn't attach."));
	}
	if(left_wheel.attached()){
		debug_print(String("wheel at pin " + left_pin + " is attached."));
	}
	else{
		debug_print(String("wheel at pin " + left_pin + " didn't attach."));
	}
}
void set_speed(int x){ //sets wheel speed to x
	right_wheel.write(x);
	left_wheel.write(x);
	debug_print(String("set speed of wheels to " + x));
}
void turn(float deg) {//turns in direction of the wheel. (e.g left wheel will turn robot left on the spot...)
	/*
		simple equation to convert the degrees into how long we need
		to turn in miliseconds to turn that amount of degrees
	*/
	auto time_to_delay = deg * (time_taken_one_rotation_ms / 360);
	set_speed(0); //to be decided (full speed for the wheel ?)
	delay(time_to_delay);
	set_speed(stop_value);
	debug_print(String("turned robot using wheel at pin " + pin + ", for " + deg));
}
void forward(int speed, float dist_cm) //taking input of desired distance in cm's{	
	auto time_to_delay = dist * (time_taken_travel_30cm / 30);
	set_speed(speed);
	delay(time_to_delay);
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
