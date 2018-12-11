

// a method to print debug information when DEBUG is defined in the code,
// if it isn't this function does nothing when called
// should change to c standard char's instead of String for preformance
// improvements...
void debug_print(String &msg) { // takes a string as input
#ifdef DEBUG
  String new_msg = String("[DEBUG]: "); // cleaning text up
  new_msg =
      new_msg + msg; // inserting our text onto the default debug print text
  Serial.println(
      new_msg); // printing our debug message and then starting a new line.
#endif
}
