#include "Receiver.hpp"
#include "ServoController.hpp"

//serial output. 0 = off, 1 = status/output display, 2 = show PWM values (for calibrating receiver)
const int serial_output = 2;

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
ServoController servos = ServoController(
  22,    //esc_left: connect to left ESC signal wire
  24,    //esc_right: right ESC signal wire
  false, //channel_swap: swaps left/right output channels
  serial_output
);

//most ESCs will provide ~6V to the receiver which you can also use to power the arduino. Connect red wire from ESC or receiver to +5V header or barrel connector on arduino
Receiver throttle = Receiver( 
  //these are the values as measured for an HPI RF-40 R/C receiver. use serial_output = 2 to determine the correct values for your receiver
  1500, //neutral_upper: these values represent the PWM width the receiver outputs while in neutral positions.
  1400, //neutral_lower: it fluctuates so we specify a range (upper and lower limits)
  1900, //limit_upper: these next values represent the entire range of input
  1000, //limit_lower
  //these are the output values from the receiver for when the transmitter is off or out of range. make sure you use a range that will capture the state consistently (it can fluctuate)
  1700, //lostcon_upper
  1600, //lostcon_lower
  28, //rx_input: signal wire for receiver throttle (usually channel 2)
  difference,
  rate_change_limit
);

Receiver steer = Receiver(
  //these are the values as measured for an HPI RF-40 R/C receiver. use serial_output = 2 to determine the correct values for your receiver
  1550, //neutral_upper: if you can't get your bot to stay still with throttle/steering in neutral, widen these ranges
  1400, //neutral_lower
  1800, //limit_upper
  1100, //limit_lower: don't forget to set the values in the next section for when the TX is disconnected
  //these are the output values from the receiver for when the transmitter is off or out of range. make sure you use a range that will capture the state consistently (it can fluctuate)
  1200, //lostcon_upper
  1000, //lostcon_lower
  30, //rx_input receiver steering (usually channel 1)
  difference,
  rate_change_limit
);

