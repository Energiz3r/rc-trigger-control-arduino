//--------------------------------------------------------------
//  This script lets you use a trigger-style R/C controller
//    to control a tank-style bot with left/right drivers
//--------------------------------------------------------------
// v1.0
//TO DO:
// -Add support for per-channel upper/lower/centre limits (useful if ESCs or motors are not identical)

#include "config.h"

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

//tracks throt/steer positions for rate change limiting
int throtpos_last = 0;
int steerpos_last = 0;

void loop() {

  throtpos = pulseIn(rx_throt_input, HIGH, 25000); //read the pulse width of the channel. 25000 is more than sufficient sample size for highly responsive I/O
  if (rate_change_limit)
  {
    if (throtpos > throtpos_last) { if ((throtpos - throtpos_last) > difference) { throtpos = throtpos_last + difference; } }
    if (throtpos < throtpos_last) { if ((throtpos_last - throtpos) > difference) {throtpos = throtpos_last - difference; } }
  }
  if (throtpos > throt_upper) { throtpos = throt_upper; } //clamp the values to within the useable range
  if (throtpos < throt_lower) { throtpos = throt_lower; }
  throtpos_last = throtpos;
  steerpos = pulseIn(rx_steer_input, HIGH, 25000);
  if (rate_change_limit)
  {
    if (steerpos > steerpos_last) { if ((steerpos - steerpos_last) > difference) {steerpos = steerpos_last + difference; } }
    if (steerpos < steerpos_last) { if ((steerpos_last - steerpos) > difference) {steerpos = steerpos_last - difference; } }
  }
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
  
  //if the channels are swapped
  if (channel_swap)
  {
    
    //set the outputs to match the movements calculated above
    leftout.write(right_channel);
    rightout.write(left_channel);
    
  }
  else
  {
  
    //set the outputs to match the movements calculated above
    leftout.write(left_channel);
    rightout.write(right_channel);
    
  }
  
}

//that's a wrap. go get coffee
