#include "debug.hpp"

class c_ir{
private:
	int reciever_pin, output_pin;
void send_signal(float frequency) {
	tone(output_pin, frequency); //sends out a ir signal @ defined frequency
}
void stop_signal() {
	noTone(output_pin); //stops outputting signal
};
public:
	void setup(int _reciever_pin, int _transmitter_pin) {
		reciever_pin = _reciever_pin;
		output_pin = _output_pin;
		pinMode(output_pin, OUTPUT);
		pinMode(reciever_pin, INPUT);
	};
	bool read() { //sends a signal then stops and returns the value gained(0 for low (no object), 1 for high (object))
		send_signal(380000); //38kHz signal, only signal the reciever works at.
		delay(500); //delay between sending signal and stopping...
		auto val = digitalRead(reciever_pin); //read value 0 for no object 1 for object.
		stop_signal();
		return val;
	}
	bool is_blocked() { //returns true if object is in way, false if not.
		return read();
	}
};
