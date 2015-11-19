#include <Servo.h>
#include "Receiver.hpp"

//serial output. 0 = off, 1 = status/output display, 2 = show PWM values (for calibrating receiver)
const int serial_output = 2;

//swaps left/right output channels
const bool channel_swap = false;

//reverse steering direction while moving forward
const bool steering_swap = false;
//reverse steering direction while moving in reverse
const bool steering_reverse_swap = false;

//the maximum difference (in PWM value) each cycle to allow the input to change by. This prevents spikes in PWM input from instantly stopping or changing wheel direction (smoothes stop/go)
const int difference = 50;
//enable or disable this setting
const bool rate_change_limit = true;

//this value alters the neutral position and upper/lower limits of the ESC output range
const float outputcentre = 90; //limits are usually 0-180 degrees where 90 is neutral for an ESC. adjust if bot moves while throttle is neutral
const float outputmax = 130; //the upper/lower values shouldn't need to be changed unless your ESC is strange
const float outputmin = 40; //I am using 130 & 40 as the ESC cuts out when the full range is used. defaults = 90, 180, 0

//alter this to change how the wheels on the turning side will behave
//a setting of 2 will set the turning side to full reverse when steering is at full lock (allows the bot to turn on the spot)
//a setting of 1 won't reverse the wheels on the turning side, only stop them (bot will turn in an arc)
//supports float values between 0 and 2
const float steering_sensitivity = 2; //default value is 2

//this value alters the sensitivity of the safety cut off (when receiver loses connection to transmitter). you shouldn't need to change this
//this works by detecting when steering and throttle are both within a certain range - the values the receiver outputs while the transmitter is turned off
//your receiver may behave differently - if that's the case you can ignore this setting.
const int lostconwaittime = 500; //time (in ms) to wait before setting outputs to neutral positions if receiver is outputting these ranges continuously

//configure digital pins on arduino
const int esc_left_output = 22; //connect to left ESC signal wire
const int esc_right_output = 24; //right ESC signal wire

//most ESCs will provide ~6V to the receiver which you can also use to power the arduino. Connect red wire from ESC or receiver to +5V header or barrel connector on arduino
Receiver throttle = { 
  //these are the values as measured for an HPI RF-40 R/C receiver. use serial_output = 2 to determine the correct values for your receiver
  neutral_upper: 1500, //these values represent the PWM width the receiver outputs while in neutral positions.
  neutral_lower: 1400, //it fluctuates so we specify a range (upper and lower limits)
  limit_upper: 1900, //these next values represent the entire range of input
  limit_lower: 1000, 
  //these are the output values from the receiver for when the transmitter is off or out of range. make sure you use a range that will capture the state consistently (it can fluctuate)
  lostcon_upper: 1700, 
  lostcon_lower: 1600, 
  rx_input: 28, //signal wire for receiver throttle (usually channel 2)
  rate_change_limit: rate_change_limit
};

Receiver steer = {
  //these are the values as measured for an HPI RF-40 R/C receiver. use serial_output = 2 to determine the correct values for your receiver
  neutral_upper: 1550, //if you can't get your bot to stay still with throttle/steering in neutral, widen these ranges
  neutral_lower: 1400,
  limit_upper: 1800,
  limit_lower: 1100, //don't forget to set the values in the next section for when the TX is disconnected
  //these are the output values from the receiver for when the transmitter is off or out of range. make sure you use a range that will capture the state consistently (it can fluctuate)
  lostcon_upper: 1200,
  lostcon_lower: 1000,
  rx_input: 30, //receiver steering (usually channel 1)
  rate_change_limit: rate_change_limit
};

