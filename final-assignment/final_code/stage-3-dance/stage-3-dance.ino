/*
  objectives:
  ->drive forwards untill barcode is encountered
  -> read barcode
  -> decode what the barcode stands for:
  -left, right, uturn, stop
  ->set leds to action required, do the turn/stop action
  ->if action isn't stop then once done robot should continue forwards untill
  another barcode is encountered


  -> record and store data on how lighting and other things effect the ldr
  sensor values
  -> comments on the data

  ->if object is detected
  ->switch on all leds
  ->move forward 25cm
  ->turn to the right 90 degrees
  ->moves forward 25cm
  ->turn to the left 90 degrees
  ->switch off all leds
  ->continue forwards

  ->boogie barcode
  ->20 seconds dance
  ->once done continue forward...
  ->flash leds whilst dancing

*/

// debug is defined since we are debugging the code currently.
#define DEBUG

// some classes made to make using our robot functions easier and
// to make debugging our code easier
#include "button.hpp"
#include "debug.hpp"
#include "ldr.hpp"
#include "led.hpp"
#include "servos.hpp"
#include "ir.hpp"

// constant's -> pins for the robot and some values needed for calcuations later
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
const float time_for_full_rotation_ms = 300; // to be set
const float time_taken_to_travel_30cm = 400; // to be set
const short ir_reciever_pin = 2;
const short ir_transmitter_pin = 3;

// pre defined object names
c_ldr ldr1;
c_ldr ldr2;
c_ldr ldr3;
c_button left_button;
c_button right_button;
c_led green_led;
c_led red_led;
c_led yellow_led;
c_servos servos;
c_ir ir;

// an enum so we can use a simple switch case later using the defines of STOP
// RIGHT LEFT UTURN and BOOGIE
enum barcode_actions { INVALID, STOP, RIGHT, LEFT, UTURN, BOOGIE};

// calibrating all 3 ldr's together for light values
void calibrate_ldrs_light() {
  debug_print("calibrating ldrs for light values");
  ldr1.calibrate(true); // true since we are calibrating light values....
  ldr2.calibrate(true);
  ldr3.calibrate(true);

  // adding all ldr values together and diving by 3 to gain an average
  // lowest light value;
  auto average_lowest_val =
    ((ldr1.get_lowest_light_value() + ldr2.get_lowest_light_value() +
      ldr3.get_lowest_light_value()) /
     3);
  // adding all ldr values together and diving by 3 to gain an average
  // highest light value;
  debug_print("average lowest light value : " + String(average_lowest_val));
  auto average_highest_val =
    (ldr1.get_highest_light_value() + ldr2.get_highest_light_value() +
     ldr3.get_highest_light_value()) /
    3;
  debug_print("average lowest light value : " + String(average_lowest_val));

  // setting each ldr's lowest and highest values for light

  ldr1.set_lowest_light_value(average_lowest_val);
  ldr2.set_lowest_light_value(average_lowest_val);
  ldr3.set_lowest_light_value(average_lowest_val);

  ldr1.set_highest_light_value(average_highest_val);
  ldr2.set_highest_light_value(average_highest_val);
  ldr3.set_highest_light_value(average_highest_val);
}

// calibrating all 3 ldr's together for dark values
void calibrate_ldrs_dark() {
  debug_print("calibrating ldrs for dark values");
  ldr1.calibrate(false);
  ldr2.calibrate(false);
  ldr3.calibrate(false);

  // adding all ldr values together and diving by 3 to gain an average
  // lowest dark value
  auto average_lowest_val =
    (ldr1.get_lowest_dark_value() + ldr2.get_lowest_dark_value() +
     ldr3.get_lowest_dark_value()) /
    3;
  debug_print("average lowest dark value : " + String(average_lowest_val));
  // adding all ldr values together and diving by 3 to gain an average
  // highest dark value;
  auto average_highest_val =
    (ldr1.get_highest_dark_value() + ldr2.get_highest_dark_value() +
     ldr3.get_highest_dark_value()) /
    3;
  debug_print("average highest dark value : " + String(average_highest_val));

  // setting all 3 ldr's lowest and highest values

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
        barcodes format : DL = Double Line, SL = Single Line, SS = Single
  Spacing left : DL, SS, DL, SS, SL, SS, SL right : SL, SS, DL, SS, SL, SS, DL
        uturn : SL, SS, SL, SS, DL, SS, SL
        stop : SL, SS, DL, SS, DL, SS, SL
        boogie: DL, SS, SL, SS, SL, SS, DL

        7 inputs to take into account take away either spacing or the lines and
  then figure out what action to do. ignore the double lines..

        Decide how often do we need to check for strips (how quickly do we get
  through one strip... tbd...)

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

  if (strip_counter == 8) { // finished reading whole barcode
    // time_taken dark strips = 1,3,5,7
    unsigned long longest_strip_time;
    unsigned long second_longest_strip_time;
    unsigned long shortest_strip_time;
    unsigned long second_shortest_strip_time;
    short longest_strip, second_longest_strip, shortest_strip,
          second_shortest_strip;

    // a for loop to help us determine our strips in order of presumed length
    // using the times stored earlier, this is prone to errors due to reading
    // strips incorrectly etc but should give us a accurate representation if a
    // strip is a double or single strip
    for (auto i = 0; i < 8;
         ++i) { //++i since we dont need to read 0 anyway (white strip)
      if (i != 2 && i != 4 && i != 6) { // filter out light strips since we are
        // comparing dark to identify strips
        if (longest_strip_time < time_taken[i]) {
          longest_strip_time = time_taken[i];
          longest_strip = i;
        } else if (second_longest_strip_time < time_taken[i]) {
          second_longest_strip_time = time_taken[i];
          second_longest_strip = i;
        } else if (second_shortest_strip_time < time_taken[i]) {
          second_shortest_strip_time = time_taken[i];
          second_shortest_strip = i;
        } else {
          shortest_strip_time = time_taken[i];
          shortest_strip = i;
        }
      }
    }

    /*
      DOUBLE STRIPS:
        left = 1 and 3
        right = 3 and 7
        uturn = 5
        stop = 3 and 5
        (boogie  = 1 and 7)
    */
    if (longest_strip == 1 && second_longest_strip == 3) {
      do_action(LEFT);
    } else if (longest_strip == 3 && second_longest_strip == 1) {
      do_action(LEFT);
    } else if (longest_strip == 3 && second_longest_strip == 7) {
      do_action(RIGHT);
    } else if (longest_strip == 7 && second_longest_strip == 3) {
      do_action(RIGHT);
    } else if (longest_strip == 3 && second_longest_strip == 5) {
      do_action(STOP);
    } else if (longest_strip == 5 && second_longest_strip == 3) {
      do_action(STOP);
    } else if (longest_strip == 1 && second_longest_strip == 7){
      do_action(BOOGIE);
    } else if(longest_strip == 7 && second_longest_strip == 1){
      do_action(BOOGIE);
    } else if (longest_strip == 5 || second_longest_strip == 5) {
      do_action(UTURN);
    } else {
      do_action(INVALID);
    }
    strip_counter = 0;
    first_input = !first_input; // reseting for a new barcode
  }
}

if (first_input) { // our first strip of the barcode
  current_strip = read_current_strip();
  strip_start_time = millis();
  ++strip_counter; // moving onto next strip therefore increase counter
  first_input = !first_input;
}

if (current_strip != read_current_strip()) { // new strip of barcode
  // how long the previous strip was read for (to detect
  // if it was a double or single strip
  time_taken[strip_counter] = millis() - strip_start_time;
  debug_print("on strip counter: " + String(strip_counter));
  ++strip_counter; // moving onto next strip therefore increase counter
  strip_start_time = millis(); // setting new strips start time
  current_strip =
    read_current_strip(); // setting our current strip to what we just read.
}
}

bool read_current_strip() { // returns true if surface is light false if not...
  if (ldr1.is_light() && ldr2.is_light() &&
      ldr3.is_light()) { // we see light surface with 99.99...% certainty
    debug_print("light surface (99.99...%)");
    return true;
  } else if ((ldr1.is_light() && ldr2.is_light()) ||
             (ldr1.is_light() && ldr3.is_light()) ||
             (ldr2.is_light() &&
              ldr3.is_light())) { // we see light surface with 66.67% certainty
    // therefore dark with 33.33...% certainty
    debug_print("light surface (66.67%)");
    return true;
  } else if (ldr1.is_light() || ldr2.is_light() ||
             ldr3.is_light()) { // we see light surface with 33.33...% certainty
    // therefore dark with 66.67% certainty
    debug_print("dark surface (66.67...%)");
    return false;
  } else { // we see dark surface with 99.99...% certainty
    debug_print("dark surface (99.99%)");
    return false;
  }
}

void boogie_action(){
  set_leds(1,0,0);
  servos.turn(90, false);
  set_leds(1,1,0);
  servos.turn(180, true);
  set_leds(1,1,1);
  servos.turn(90,true);
  set_leds(0,0,0);
  servos.turn(90,false);
  set_leds(0,0,1);
  servos.turn(180,true);
  set_leds(0,1,1);
  servos.turn(180,true);
  set_leds(1,1,1);
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
      while (1) { // finished program so entering endless loop (cleanest way of
        delay(50) // "exiting" arduino...)
      };
      break;
    case RIGHT:
      debug_print("Turn right action provided, turning 90° clockwise, then "
                  "continue forward...");
      set_leds(0, 1, 0);
      servos.turn(90, true);
      servos.set_speed(2, 3);
      break;
    case LEFT:
      debug_print("Turn left action provided, turning 90° anti-clockwise, then "
                  "continue forward...");
      set_leds(1, 0, 0);
      servos.turn(90, false);
      servos.set_speed(2, 3);
      break;
    case UTURN:
      debug_print("U-Turn action provided, turning 180° clockwise, then continue "
                  "forward...");
      set_leds(1, 1, 0);
      servos.turn(180, true);
      servos.set_speed(2, 3);
      break;
    case BOOGIE:
      debug_print("boogie action provided, dancing for 20 seconds and flashing leds, then continue forward...");
      unsigned long end_time = millis() + 20000; //20 seconds of dancing
      while(millis() < end_time) {
        boogie_action();
      }
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
  servos.setup(right_servo_pin, left_servo_pin, right_servo_stop_value,
               left_servo_stop_value, time_for_full_rotation_ms,
               time_taken_to_travel_30cm);
  ir.setup(int ir_reciever_pin, int ir_transmitter_pin);
  debug_print("finished setting up...");

  servos.stop();
  // do calibrating
  set_leds(0, 0, 1); // red to show left button needs to be pressed
  left_button
  .wait(); // wait for left button to be pressed before starting calibration
  calibrate_ldrs_light();
  set_leds(0, 1, 0); // yellow to show right button needs to be pressed
  left_button
  .wait(); // wait for left button to be pressed before starting calibration
  calibrate_ldrs_dark();
  set_leds(1, 0, 0); // done calibrating starting once led is off and start
  // button is pressed (left button again)
  delay(500);
  left_button.wait(); // waiting for start button to be pressed
  set_leds(0, 0, 0);
  delay(1000);
  servos.set_speed(2, 3); // start moving forward at speed of then go into loop.
}

void loop() {
  static bool obstacle_avoidance_action = true;
  while(!ir.is_blocked(){
      follow_barcode();
      obstacle_avoidance_action = true;
  }
  if(obstacle_avoidance_action){
    set_leds(1,1,1);
    debug_print("obstacle detected....");
    servos.turn(90, true);
    servos.forward(5,5,25);
    servos.turn(90, false);
    set_leds(0,0,0);
    delay(500);
    obstacle_avoidance_action = false;
  }
}
