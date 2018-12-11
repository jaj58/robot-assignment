#include "debug.hpp"

class c_button{
	private:
	int pin;

	public:
	void setup(int _pin){
		pin = _pin;
		pinMode(pin, INPUT);
	}
	void wait(){ //waits for button press...
		debug_print("waiting for button press...");
		 while (digitalRead(button_pin) == HIGH) {
		 }
		 delay(20);
		 debug_print("button pressed, waiting for release...");
		 while(digitalRead(button_pin) == LOW{

		 }
		 debug_print("button released, returning...");
		 delay(20);
		 return;
	}
};
