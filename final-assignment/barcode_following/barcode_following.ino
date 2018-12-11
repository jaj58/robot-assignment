/*
  objectives:
  -> read barcode
  -> decide what the barcode stands for:
  -left, right, uturn, stop, invalid
  -> record and store data on how lighting and other things effect the ldr sensor values
  -> comments on the data

  actions:
  invalid = 0
  stop = 1
  right = 2
  left = 3
  uturn = 4

*/

#define DEBUG

#include "debug.hpp"
#include "ldr.hpp"
#include "button.hpp"
#include "led.hpp"
#include "servos.hpp"


#define ldr1_pin A0
#define ldr2_pin A1
#define ldr3_pin A2
const short left_button_pin = 4;
const short right_button_pin = 2;
const short red_led_pin = 13;
const short yellow_led_pin = 12;
const short green_led_pin = 7;
const short right_servo_pin = 5;
const short left_servo_pin = 6;
const short right_servo_stop_value = 86;
const short left_servo_stop_value = 80;
const float time_for_full_rotation_ms = 300; //to be set
const float time_taken_to_travel_30cm = 0; //to be set

c_ldr ldr1;
c_ldr ldr2;
c_ldr ldr3;
c_button left_button;
c_button right_button;
c_led green_led;
c_led red_led;
c_led yellow_led;
c_servos servos;

enum barcode_actions {
  INVALID,
  STOP,
  RIGHT,
  LEFT,
  UTURN
};

void calibrate_ldrs_light() {
  debug_print("calibrating ldrs for light values");
  ldr1.calibrate(true);
  ldr2.calibrate(true);
  ldr3.calibrate(true);

  auto average_lowest_val = (ldr1.get_lowest_light_value() + ldr2.get_lowest_light_value() + ldr3.get_lowest_light_value()) / 3; //adding all ldr values together and diving by 3 to gain an average lowest light value;
  String temp = String("average lowest light value : ");
  temp = temp + average_lowest_val;
  debug_print(temp);
  auto average_highest_val = (ldr1.get_highest_light_value() + ldr2.get_highest_light_value() + ldr3.get_highest_light_value()) / 3; //adding all ldr values together and diving by 3 to gain an average highest light value;
  String temp2 = String("average highest light value : ");
  temp2 = temp2 + average_highest_val;
  debug_print(temp2);

  ldr1.set_lowest_light_value(average_lowest_val);
  ldr2.set_lowest_light_value(average_lowest_val);
  ldr3.set_lowest_light_value(average_lowest_val);

  ldr1.set_highest_light_value(average_highest_val);
  ldr2.set_highest_light_value(average_highest_val);
  ldr3.set_highest_light_value(average_highest_val);
}

void calibrate_ldrs_dark() {
  debug_print("calibrating ldrs for dark values");
  ldr1.calibrate(false);
  ldr2.calibrate(false);
  ldr3.calibrate(false);

  auto average_lowest_val = (ldr1.get_lowest_dark_value() + ldr2.get_lowest_dark_value() + ldr3.get_lowest_dark_value()) / 3; //adding all ldr values together and diving by 3 to gain an average lowest dark value;
  String temp = String("average lowest dark value : ");
  temp = temp + average_lowest_val;
  debug_print(temp);
  auto average_highest_val = (ldr1.get_highest_dark_value() + ldr2.get_highest_dark_value() + ldr3.get_highest_dark_value()) / 3; //adding all ldr values together and diving by 3 to gain an average highest dark value;
  String temp2 = String("average highest dark value : ");
  temp2 = temp2 + average_highest_val;
  debug_print(temp2);

  ldr1.set_lowest_dark_value(average_lowest_val);
  ldr2.set_lowest_dark_value(average_lowest_val);
  ldr3.set_lowest_dark_value(average_lowest_val);

  ldr1.set_highest_dark_value(average_highest_val);
  ldr2.set_highest_dark_value(average_highest_val);
  ldr3.set_highest_dark_value(average_highest_val);
}

void set_leds(bool green, bool yellow, bool red) {
  green_led.set_led(green);
  yellow_led.set_led(yellow);
  red_led.set_led(red);
}

/*
	barcodes format : DL = Double Line, SL = Single Line, SS = Single Spacing
	left : DL, SS, DL, SS, SL, SS, SL
	right : SL, SS, DL, SS, SL, SS, DL
	uturn : SL, SS, SL, SS, DL, SS, SL
	stop : SL, SS, DL, SS, DL, SS, SL
	boogie: DL, SS, SL, SS, SL, SS, DL

	7 inputs to take into account take away either spacing or the lines and then figure out what action to do.
	ignore the double lines..

	Decide how often do we need to check for strips (how quickly do we get through one strip... tbd...)

  unisnged long end_time = millis() + time_wanted;
  while(millis() < end_time){
  //do stuff
  }
  if(millis() >= end_time){
  //times up
  }
*/

void follow_barcode() {
  static bool first_input = true;
  static int strip_counter = 0;
  unsigned long time_taken[8];
  static bool current_strip = false;
  unsigned long strip_start_time;

  if (strip_counter > 8) { //finished reading whole barcode
    //time_taken dark strips = 1,3,5,7
    unsigned long longest_strip_time; //will always be a double strip
    unsigned long second_longest_strip_time; //most likely a double strip except for uturn barcode (only contains one double strip)
    unsigned long shortest_strip_time; //always a single strip
    unsigned long second_shortest_strip_time; //always a single strip
    short longest_strip, second_longest_strip, shortest_strip, second_shortest_strip;

    for (auto i = 0; i < 8; ++i) { //++i since we dont need to read 0 anyway
      if (i != 2 && i != 4 && i != 6) { //filter out light strips since we are comparing dark...
        if (longest_strip_time < time_taken[i]) {
          longest_strip_time = time_taken[i];
          longest_strip = i;
        }
        else if (second_longest_strip_time < time_taken[i]) {
          second_longest_strip_time = time_taken[i];
          second_longest_strip = i;
        }
        else if (second_shortest_strip_time < time_taken[i]) {
          second_shortest_strip_time = time_taken[i];
          second_shortest_strip = i;
        }
        else {
          shortest_strip_time = time_taken[i];
          shortest_strip = i;
        }
      }
    }
    unsigned long dif1 = longest_strip_time - second_longest_strip_time;
    unsigned long dif2 = second_longest_strip_time - second_shortest_strip_time;
    static bool strip_double;
    if (dif1 > dif2) {//most likely a single strip..
      strip_double = false;
    }
    else {//most likely a double strip
      strip_double = true;
    }
    // if (strip_double) { //we have 2 double strips therefore we have 3 options stop,right or left
    /*
        strips : 1, 3, 5 or 7
    */
    if (longest_strip == 1 || second_longest_strip == 1 || longest_strip == 3 || second_longest_strip == 3) { //if strip 1 is a double strip we know its the left barcode
      do_action(LEFT);
    }
    else if (longest_strip == 7 || second_longest_strip == 7 || longest_strip == 3 || second_longest_strip == 3) { //if strip 7 is a double strip we know its the right barcode
      do_action(RIGHT);
    }
    else if (longest_strip == 3 || second_longest_strip == 3 || longest_strip == 5 || second_longest_strip == 5) {
      do_action(STOP);
    }
    // }
    // else { //no second double strip
    //since only one barcode doesnt have 2 double strips it must be the uturn barcode therefore...
    else  if (longest_strip == 5 || second_longest_strip == 5) {
      do_action(UTURN);
    }
    else {
      do_action(INVALID);
    }
    // }
    strip_counter = 0;
    first_input = !first_input; //reseting for a new barcode
  }

  if (first_input) { //our first strip of the barcode
    current_strip = read_current_strip();
    strip_start_time = millis();
    ++strip_counter; //moving onto next strip therefore increase counter
    first_input = !first_input;
  }

  if (current_strip !=  read_current_strip()) { //new strip of barcode
    time_taken[strip_counter] = millis() - strip_start_time; //how long the previous strip was read for (to detect if it was a double or single strip
    String dbg = String("on strip counter: ");
    dbg = dbg + strip_counter;
    debug_print(dbg);
    ++strip_counter; //moving onto next strip therefore increase counter
    strip_start_time = millis(); //setting new strips start time
    current_strip = read_current_strip();
  }
}

bool read_current_strip() { //returns true if surface is light false if not...
  /*
    if (ldr1.is_light() && ldr2.is_light() && ldr3.is_light()) { //we see light surface with 99.99...% certainty
      delay(200);
      if (ldr1.is_light() && ldr2.is_light() && ldr3.is_light()) { //checking twice
        delay(200);
        if (ldr1.is_light() && ldr2.is_light() && ldr3.is_light()) {
          return true;
        };
      }
    }
    else if ((ldr1.is_light() && ldr2.is_light()) ||
             (ldr1.is_light() && ldr3.is_light()) ||
             (ldr2.is_light() && ldr3.is_light())) { //we see light surface with 66.67% certainty therefore dark with 33.33...% certainty
      delay(200);
      if ((ldr1.is_light() && ldr2.is_light()) ||
          (ldr1.is_light() && ldr3.is_light()) ||
          (ldr2.is_light() && ldr3.is_light()))
      {
        delay(200);
        if ((ldr1.is_light() && ldr2.is_light()) ||
            (ldr1.is_light() && ldr3.is_light()) ||
            (ldr2.is_light() && ldr3.is_light()))
        {
          return true;
        }
      }

    }
    else if (ldr1.is_light() || ldr2.is_light() || ldr3.is_light()) {
      delay(200);
      if (ldr1.is_light() || ldr2.is_light() || ldr3.is_light()) {
        delay(200);
        if (ldr1.is_light() || ldr2.is_light() || ldr3.is_light()) {
          return false;
        }
      }
    }
    else {
      return false;
    }
  */


  if (ldr1.is_light() && ldr2.is_light() && ldr3.is_light()) { //we see light surface with 99.99...% certainty
    debug_print("light surface (99.99...%)");
    return true;
  }
  else if ((ldr1.is_light() && ldr2.is_light()) ||
           (ldr1.is_light() && ldr3.is_light()) ||
           (ldr2.is_light() && ldr3.is_light())) { //we see light surface with 66.67% certainty therefore dark with 33.33...% certainty
    debug_print("light surface (66.67%)");
    return true;
  }
  else if (ldr1.is_light() || ldr2.is_light() || ldr3.is_light()) { //we see light surface with 33.33...% certainty therefore dark with 66.67% certainty
    debug_print("dark surface (66.67...%)");
    return false;
  }
  else { //we see dark surface with 99.99...% certainty
    debug_print("dark surface (99.99%)");
    return false;
  }

}


void do_action(int action) {
  switch (action) {
    case INVALID:
      debug_print("invalid action provided, ignoring...");
      servos.set_speed(2, 3);
    case STOP:
      debug_print("Stop action provided, stopping robot...");
      set_leds(0, 0, 1);
      servos.stop();
      while (1) {};
      break;
    case RIGHT:
      debug_print("Turn right action provided, turning 90° clockwise, then continue forward...");
      set_leds(0, 1, 0);
      servos.turn(90, true);
      servos.set_speed(2, 3);
      break;
    case LEFT:
      debug_print("Turn left action provided, turning 90° anti-clockwise, then continue forward...");
      set_leds(1, 0, 0);
      servos.turn(90, false);
      servos.set_speed(2, 3);
      break;
    case UTURN:
      debug_print("U-Turn action provided, turning 180° clockwise, then continue forward...");
      set_leds(1, 1, 0);
      servos.turn(180, true);
      servos.set_speed(2, 3);
      break;
  }
  return;
}

void setup() {
  Serial.begin(9600);
  ldr1.setup(ldr1_pin);
  ldr2.setup(ldr2_pin);
  ldr3.setup(ldr3_pin);
  left_button.setup(left_button_pin);
  right_button.setup(right_button_pin);
  green_led.setup(green_led_pin);
  red_led.setup(red_led_pin);
  yellow_led.setup(yellow_led_pin);
  servos.setup(right_servo_pin, left_servo_pin, right_servo_stop_value, left_servo_stop_value, time_for_full_rotation_ms, time_taken_to_travel_30cm);
  debug_print("finished setting up...");

  servos.stop();
  //do calibrating
  set_leds(0, 0, 1); //red to show left button needs to be pressed
  left_button.wait();  //wait for left button to be pressed before starting calibration
  calibrate_ldrs_light();
  set_leds(0, 1, 0); //yellow to show right button needs to be pressed
  left_button.wait(); //wait for left button to be pressed before starting calibration
    calibrate_ldrs_dark();
  set_leds(1, 0, 0); //done calibrating starting once led is off and start button is pressed (left button again)
  delay(500);
  left_button.wait();  //waiting for start button to be pressed
  set_leds(0, 0, 0);
  delay(1000);
  servos.set_speed(2, 3); //start moving forward at speed of 10(can be changed...) then go into loop.
}

void loop() {
  follow_barcode();
}
