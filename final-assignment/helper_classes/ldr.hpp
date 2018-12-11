#include "debug.hpp"

class c_ldr{
	private:
	int pin;
	int lowest_light_val, highest_light_val, lowest_dark_val, highest_dark_val;
	public:
	void setup(int _pin){
		pin = _pin;
		pinMode(pin, INPUT);
	}
	int read(){
		return analogRead(pin);
	}
	void calibrate(bool light){ //false for dark calibration
		int temp_val = read();
		if(light){
			if (temp_val > highest_light_val){
				highest_light_val = temp_val;
				debug_print(String("highest light value for ldr pin " + pin + " set to " + highest_light_val));
			}
			else if(temp_val < lowest_light_val){
				lowest_light_val = temp_val;
				debug_print(String("lowest light value for ldr pin " + pin + " set to " + lowest_light_val));
			}
		}
		else{
			if (temp_val > highest_dark_val){
				highest_dark_val = temp_val;
				debug_print(String("highest dark value for ldr pin " + pin + " set to " + highest_dark_val));
			}
			else if(temp_val < lowest_dark_val){
				lowest_dark_val = temp_val;
				debug_print(String("lowest dark value for ldr pin " + pin + " set to " + lowest_dark_val));
			}
		}
		debug_print("finished calibrating ldr");
	}
	bool is_light(){//check to see if ldr sees light(true) or dark(false)
		auto val = read();
		if(lowest_light_val < val && val < highest_light_val){
			debug_print("ldr detected light");
			return true;
		}
		else if(lowest_dark_val < val && val < highest_dark_val){
			debug_print("ldr detected dark");
			return false;
		}
		else{
			/*
			this should probably be changed to decide what the read value is closer
			to or if we should read again to see if there was an error/ inconsistency
			*/
			debug_print("Invalid value, Should re-calibrate lowest and highest values.");
		}
	}
	//getters
	int get_lowest_light_value(){
		return lowest_light_val;
	}
	int get_lowest_dark_value(){
		return lowest_dark_val;
	}
	int get_highest_light_value(){
		return highest_light_val;
	}
	int get_highest_dark_value(){
		return highest_dark_val;
	}
	//setters
	void set_lowest_light_value(int val){
		lowest_light_val = val;
	}
	void set_lowest_dark_value(int val){
		lowest_dark_val = val;
	}
	void set_highest_light_value(int val){
		highest_light_val = val;
	}
	void set_highest_dark_value(int val){
		highest_dark_val = val;
	}
};
