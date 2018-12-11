#ifdef DEBUG

void debug_print(String msg) { //just a simple printing method that only works when DEBUG is defined in the program.
	String new_msg = String("[DEBUG]: ");
	new_msg = new_msg + msg;
	Serial.println(new_msg);
}

#endif
