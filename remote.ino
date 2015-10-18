//--------------------------------------------------------------
//  This script lets you use a trigger-style R/C controller
//    to control a tank-style bot with left/right drivers
//--------------------------------------------------------------
// v1.0
//TO DO:
// -Add support for per-channel upper/lower/centre limits (useful if ESCs or motors are not identical)

// ---CONFIG START---

//serial output. 0 = off, 1 = status/output display, 2 = show PWM values (for calibrating receiver)
const int serial_output = 2;

//reverse steering direction while moving forward
const boolean steering_swap = true;
//reverse steering direction while moving in reverse
const boolean steering_reverse_swap = true;

//this value alters the neutral position and upper/lower limits of the ESC output range
const float outputcentre = 90; //limits are usually 0-180 degrees where 90 is neutral for an ESC. adjust if bot moves while throttle is neutral
const float outputmax = 130; //the upper/lower values shouldn't need to be changed unless your ESC is strange
const float outputmin = 40; //I am using 130 & 40 as the ESC cuts out when the full range is used. defaults = 90, 180, 0

//alter this to change how the wheels on the turning side will behave
//a setting of 2 will set the turning side to full reverse when steering is at full lock (allows the bot to turn on the spot)
//a setting of 1 won't reverse the wheels on the turning side, only stop them (bot will turn in an arc)
//supports float values between 0 and 2
const float steering_sensitivity = 2; //default value is 2

//these are the values as measured for an HPI RF-40 R/C receiver. use serial_output = 2 to determine the correct values for your receiver
const float throt_neutral_upper = 1500; //these values represent the PWM width the receiver outputs while in neutral positions.
const float throt_neutral_lower = 1400; //it fluctuates so we specify a range (upper and lower limits)
const float steer_neutral_upper = 1550; //if you can't get your bot to stay still with throttle/steering in neutral, widen these ranges
const float steer_neutral_lower = 1400;
const float throt_upper = 1900; //these next values represent the entire range of input
const float throt_lower = 1000;
const float steer_upper = 1800;
const float steer_lower = 1100; //don't forget to set the values in the next section for when the TX is disconnected

//this value alters the sensitivity of the safety cut off (when receiver loses connection to transmitter). you shouldn't need to change this
//this works by detecting when steering and throttle are both within a certain range - the values the receiver outputs while the transmitter is turned off
//your receiver may behave differently - if that's the case you can ignore this setting.
const int lostconwaittime = 500; //time (in ms) to wait before setting outputs to neutral positions if receiver is outputting these ranges continuously

//these are the output values from the receiver for when the transmitter is off or out of range. make sure you use a range that will capture the state consistently (it can fluctuate)
const int throt_lostcon_upper = 1700; //throttle
const int throt_lostcon_lower = 1600;
const int steer_lostcon_upper = 1200; //steering
const int steer_lostcon_lower = 1000;

//configure digital pins on arduino
const int esc_left_output = 22; //connect to left ESC signal wire
const int esc_right_output = 24; //right ESC signal wire
const int rx_throt_input = 28; //signal wire for receiver throttle (usually channel 2)
const int rx_steer_input = 30; //receiver steering (usually channel 1)

//most ESCs will provide ~6V to the receiver which you can also use to power the arduino. Connect red wire from ESC or receiver to +5V header or barrel connector on arduino

// ---END CONFIG---

int throtpos = (throt_neutral_upper - throt_neutral_lower) / 2; //stores the current positions for the steering and throttle PWM values
int steerpos = (steer_neutral_upper - steer_neutral_lower) / 2;
int long tracklostcon = 0; //tracks if the connection is interrupted

//set up servo objects
#include <Servo.h>
Servo leftout; //L channel
Servo rightout; //R channel

void setup() {

  //set up pins
  leftout.attach(esc_left_output); //left channel output
  rightout.attach(esc_right_output); //right channel output
  pinMode(rx_throt_input, INPUT); //throttle input
  pinMode(rx_steer_input, INPUT); //steering input

  Serial.begin(9600); //pour a bowl of Serial

}

int throtpos_last = 0;
int steerpos_last = 0;
int difference = 50;

void loop() {

  throtpos = pulseIn(rx_throt_input, HIGH, 25000); //read the pulse width of the channel. 25000 is more than sufficient sample size for highly responsive I/O
  if (throtpos > throtpos_last) { if ((throtpos - throtpos_last) > difference) { throtpos = throtpos_last + difference; } }
  if (throtpos < throtpos_last) { if ((throtpos_last - throtpos) > difference) {throtpos = throtpos_last - difference; } }
  if (throtpos > throt_upper) { throtpos = throt_upper; } //clamp the values to within the useable range
  if (throtpos < throt_lower) { throtpos = throt_lower; }
  throtpos_last = throtpos;
  steerpos = pulseIn(rx_steer_input, HIGH, 25000);
  if (steerpos > steerpos_last) { if ((steerpos - steerpos_last) > difference) {steerpos = steerpos_last + difference; } }
  if (steerpos < steerpos_last) { if ((steerpos_last - steerpos) > difference) {steerpos = steerpos_last - difference; } }
  if (steerpos > steer_upper) { steerpos = steer_upper; }
  if (steerpos < steer_lower) { steerpos = steer_lower; }
  steerpos_last = steerpos;
  int left_channel = 0;
  int right_channel = 0;
  int neutral_safety = 0;
  
  //if the conditions for 'connection lost' are met, ie. the outputs from the receiver are within a specific range
  if (((throtpos > throt_lostcon_lower) && (throtpos < throt_lostcon_upper) && (steerpos > steer_lostcon_lower) && (steerpos < steer_lostcon_upper)))
  {
    
    //keep track of how long the connection lost conditions have been met
    if (tracklostcon == 0)
    {
      
      tracklostcon = millis();
      
      //debug output
      if (serial_output == 1) { Serial.println("Connection seems to be interrupted... "); }
      
    }
    
    //if the connection is still interrupted after the allowed time the TX is probably off or out of range
    if ((millis() - tracklostcon) > lostconwaittime)
    {
      
      //set outputs to neutral positions
      left_channel = outputcentre;
      right_channel = outputcentre;
      
      //disable normal movement
      neutral_safety = 1;
      
      //debug output
      if (serial_output == 1) { Serial.println("Connection lost!"); }
    
    }
    
  }
  else
  {
    
   //reset lost connection tracking if conditions are normal to prevent stalling unnecessarily
   tracklostcon = 0;
    
  }
  
  //only perform normal movements if the lost connection timer hasn't been exceeded
  if (neutral_safety == 0)
  {
    
    //if the throttle position is above the neutral range (forward)
    if (throtpos > throt_neutral_upper)
    {
      
      //find % of throttle applied
      float throt_percent = (((throtpos - throt_neutral_upper) / (throt_upper - throt_neutral_upper)) * 100);
      
      //find the output throttle value
      float output_throt = ((outputmax - outputcentre) / 100) * throt_percent;
      
      //if steering left
      if (steerpos > steer_neutral_upper)
      {
        
        //find % of 'steering left' applied (full left is 100%, centre is 0%)
        float steer_percent = (((steerpos - steer_neutral_upper) / (steer_upper - steer_neutral_upper)) * 100);
        
        //take that percentage, and subtract it from the throttle output for the left channel, multiplied by sensitivity so it will go in reverse at full steer
        steer_percent = outputcentre + output_throt - (steering_sensitivity * ((output_throt / 100) * steer_percent));
        int newoutput = steer_percent; //convert to int
        int regoutput = outputcentre + output_throt;
        if (steering_swap)
        {
          left_channel = newoutput; //set the output
          right_channel = regoutput;
        }
        else
        {
          right_channel = newoutput;
          left_channel = regoutput;
        }
        
        //debug output
        if (serial_output == 1) { Serial.println("(Forward) Left channel servo: " + String(newoutput)); }
        
      }
      //if steering right
      else if (steerpos < steer_neutral_lower)
      {
        
        //find % of 'steering right' applied (full right is 100%, centre is 0%)
        float steer_percent = (((steer_neutral_lower - steerpos) / (steer_neutral_lower - steer_lower)) * 100);
        
        //take that percentage, and subtract it from the throttle output for the right channel, multiplied by sensitivity so it will go in reverse at full steer
        steer_percent = outputcentre + output_throt - (steering_sensitivity * ((output_throt / 100) * steer_percent));
        int newoutput = steer_percent; //convert to int
        int regoutput = outputcentre + output_throt;
        if (steering_swap)
        {
          right_channel = newoutput; //set the output
          left_channel = regoutput;
        }
        else
        {
          left_channel = newoutput;
          right_channel = regoutput;
        }
        
        //debug output
        if (serial_output == 1) { Serial.println("(Forward) Right channel servo: " + String(newoutput)); }
        
      }
      else //if steering is neutral
      {
      
        int output = outputcentre + output_throt;
        left_channel = output;
        right_channel = output;
        
        //debug output
        if (serial_output == 1) { Serial.println("(Forward) Both servos: " + String(output)); }
      
      }
      
    }
    //if the throttle is below neutral range (reverse)
    else if (throtpos < throt_neutral_lower)
    {
      
      //find % of throttle applied
      int throt_percent = ((throt_neutral_lower - throtpos) / (throt_neutral_lower - throt_lower)) * 100;
      
      //find the output throttle value
      float output_throt = ((outputcentre - outputmin) / 100) * throt_percent;
      
      //if steering left
      if (steerpos > steer_neutral_upper)
      {
        
        //find % of 'steering left' applied (full left is 100%, centre is 0%)
        float steer_percent = (((steerpos - steer_neutral_upper) / (steer_upper - steer_neutral_upper)) * 100);
        
        //take that percentage, and subtract it from the throttle output for the left channel, multiplied by sensitivity so it will go in reverse at full steer
        steer_percent = outputcentre - output_throt + (steering_sensitivity * ((output_throt / 100) * steer_percent));
        int newoutput = steer_percent; //convert to int
        int regoutput = outputcentre - output_throt;
        if (steering_reverse_swap)
        {
          left_channel = newoutput; //set the output
          right_channel = regoutput;
        }
        else
        {
          right_channel = newoutput;
          left_channel = regoutput;
        }
        
        //debug output
        if (serial_output == 1) { Serial.println("(Forward) Left channel servo: " + String(newoutput)); }
        
      }
      //if steering right
      else if (steerpos < steer_neutral_lower)
      {
        
        //find % of 'steering right' applied (full right is 100%, centre is 0%)
        float steer_percent = (((steer_neutral_lower - steerpos) / (steer_neutral_lower - steer_lower)) * 100);
        
        //take that percentage, and subtract it from the throttle output for the right channel, multiplied by sensitivity so it will go in reverse at full steer
        steer_percent = outputcentre - output_throt + (steering_sensitivity * ((output_throt / 100) * steer_percent));
        int newoutput = steer_percent; //convert to int
        int regoutput = outputcentre - output_throt;
        if (steering_reverse_swap)
        {
          right_channel = newoutput; //set the output
          left_channel = regoutput;
        }
        else
        {
          left_channel = newoutput;
          right_channel = regoutput;
        }
        
        //debug output
        if (serial_output == 1) { Serial.println("(Forward) Left channel servo: " + String(newoutput)); }
        
      }
      else //if steering is neutral
      {
      
        int output = outputcentre - output_throt;
        left_channel = output;
        right_channel = output;
        
        //debug output
        if (serial_output == 1) { Serial.println("(Reversing) Both servos: " + String(output)); }
      
      }
      
    }
    else //neutral / idle
    {
      
      int output = outputcentre;
      left_channel = output;
      right_channel = output;
      
      //debug output
      if (serial_output == 1) { Serial.println("(Neutral) Both servos: " + String(output)); }
      
    }
    
  }
  
  //debug output
  if (serial_output == 2) { Serial.println("throttle position: " + String(throtpos) + " steering position: " + String(steerpos)); }
  
  //set the outputs to match the movements calculated above
  leftout.write(left_channel);
  rightout.write(right_channel);
  
}

//that's a wrap. go get coffee
