#include "E:\uni-work\Introduction to Programming Worksheets\final-assignment\ldr.hpp"
//ldr pins
#define ldr1_pin A0
#define ldr2_pin A1
#define ldr3_pin A2
#define left_button_pin 4
#define right_button_pin 2
c_ldr ldr1;
c_ldr ldr2;
c_ldr ldr3;
c_button left_button;
c_button right_button;

void calibrate_ldrs_light() {
  debug_print("calibrating ldrs for light values");
  ldr1.calibrate(true);
  ldr2.calibrate(true);
  ldr3.calibrate(true);

  auto average_lowest_val = (ldr1.get_lowest_light_value() + ldr2.get_lowest_light_value() + ldr3.get_lowest_light_value()) / 3; //adding all ldr values together and diving by 3 to gain an average lowest light value;
  debug_print(String("average lowest light value : " + average_lowest_val));
  auto average_highest_val = (ldr1.get_highest_light_value() + ldr2.get_highest_light_value() + ldr3.get_highest_light_value()) / 3; //adding all ldr values together and diving by 3 to gain an average highest light value;
  debug_print(String("average highest light value : " + average_highest_val));

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
  debug_print(String("average lowest dark value : " + average_lowest_val));
  auto average_highest_val = (ldr1.get_highest_dark_value() + ldr2.get_highest_dark_value() + ldr3.get_highest_dark_value()) / 3; //adding all ldr values together and diving by 3 to gain an average highest dark value;
  debug_print(String("average highest dark value : " + average_highest_val));

  ldr1.set_lowest_dark_value(average_lowest_val);
  ldr2.set_lowest_dark_value(average_lowest_val);
  ldr3.set_lowest_dark_value(average_lowest_val);

  ldr1.set_highest_dark_value(average_highest_val);
  ldr2.set_highest_dark_value(average_highest_val);
  ldr3.set_highest_dark_value(average_highest_val);
}



void setup() {
  ldr1.setup(ldr1_pin);
  ldr2.setup(ldr2_pin);
  ldr3.setup(ldr3_pin);
  left_button.setup(left_button_pin);
  right_button.setup(right_button_pin);
  debug_print("finished setting up...");
  
  //do calibrating
  left_button.wait();  //wait for left button to be pressed before starting calibration
  calibrate_ldrs_light(); 
  right_button.wait(); //wait for right button to be pressed before starting calibration
  calibrate_ldrs_dark();
}

void loop() {

}
