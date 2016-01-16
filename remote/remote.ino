//--------------------------------------------------------------
//  This script lets you use a trigger-style R/C controller
//    to control a tank-style bot with left/right drivers
//--------------------------------------------------------------
// v1.0.1b
//
// Modified to suit control of a CanaKit UK1122 Dual H-Bridge Reversible 2A Motor Controller
// Pins set up to suit Arduino Nano

// ---CONFIG START---

//serial output. 0 = off, 1 = status/output display, 2 = show PWM values (for calibrating receiver)
const int serial_output = 1;

//loop delay time
const int loop_delay = 40; //milliseconds

//swaps left/right output channels
const boolean channel_swap = false;

//reverse steering direction while moving forward
const boolean steering_swap = false;
//reverse steering direction while moving in reverse
const boolean steering_reverse_swap = false;

//the maximum difference (in PWM value) each cycle to allow the input to change by. This prevents spikes in PWM input from instantly stopping or changing wheel direction (smoothes stop/go)
const int difference = 50;
//enable or disable this setting
const boolean rate_change_limit = true;

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
const int esc_left_forward = 2;
const int esc_left_move = 3;
const int esc_left_reverse = 4;

const int esc_right_forward = 5;
const int esc_right_move = 6;
const int esc_right_reverse = 7;

const int rx_throt_input = 8; //signal wire for receiver throttle (usually channel 2)
const int rx_steer_input = 9; //receiver steering (usually channel 1)

// ---END CONFIG---

int throtpos = (throt_neutral_upper - throt_neutral_lower) / 2; //stores the current positions for the steering and throttle PWM values
int steerpos = (steer_neutral_upper - steer_neutral_lower) / 2;
int long tracklostcon = 0; //tracks if the connection is interrupted

void setup() {

  //set up pins
  pinMode(esc_left_move, OUTPUT);
  pinMode(esc_left_forward, OUTPUT);
  pinMode(esc_left_reverse, OUTPUT);
  pinMode(esc_right_move, OUTPUT);
  pinMode(esc_right_forward, OUTPUT);
  pinMode(esc_right_reverse, OUTPUT);
  pinMode(rx_throt_input, INPUT);
  pinMode(rx_steer_input, INPUT);

  Serial.begin(9600); //pour a bowl of Serial

}

//tracks throt/steer positions for rate change limiting
int throtpos_last = 0;
int steerpos_last = 0;

void loop() {
  
  //wait a while so the pulseIn works properly
  delay(loop_delay);
  
  //get the throttle position
  throtpos = pulseIn(rx_throt_input, HIGH, 25000); //read the pulse width of the channel. 25000 is more than sufficient sample size for highly responsive I/O
  if (rate_change_limit)
  {
    if (throtpos > throtpos_last) { if ((throtpos - throtpos_last) > difference) { throtpos = throtpos_last + difference; } }
    if (throtpos < throtpos_last) { if ((throtpos_last - throtpos) > difference) {throtpos = throtpos_last - difference; } }
  }
  if (throtpos > throt_upper) { throtpos = throt_upper; } //clamp the values to within the useable range
  if (throtpos < throt_lower) { throtpos = throt_lower; }
  throtpos_last = throtpos;
  
  //get the steering position
  steerpos = pulseIn(rx_steer_input, HIGH, 25000);
  if (rate_change_limit)
  {
    if (steerpos > steerpos_last) { if ((steerpos - steerpos_last) > difference) {steerpos = steerpos_last + difference; } }
    if (steerpos < steerpos_last) { if ((steerpos_last - steerpos) > difference) {steerpos = steerpos_last - difference; } }
  }
  if (steerpos > steer_upper) { steerpos = steer_upper; }
  if (steerpos < steer_lower) { steerpos = steer_lower; }
  steerpos_last = steerpos;
  
  //holds the values for the channels
  boolean left_channel = false; //if the channel is moving
  boolean left_channel_reverse = false; //if the direction is reverse (otherwise it's forward)
  boolean right_channel = false;
  boolean right_channel_reverse = false;
  
  //if the receiver is in failsafe
  boolean neutral_safety = false;
  
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
      left_channel = false;
      right_channel = false;
      
      //disable normal movement
      neutral_safety = true;
      
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
  if (!neutral_safety)
  {
    
    //if the throttle position is above the neutral range (forward)
    if (throtpos > throt_neutral_upper)
    {
      
      right_channel = true;
      left_channel = true;
      
      //if steering right
      if (steerpos > steer_neutral_upper)
      {
        if (!steering_swap) {
          right_channel_reverse = true;
          if (serial_output == 1) { Serial.println("Forward + Turning Right"); }
        }
        else {
          left_channel_reverse = true;
          if (serial_output == 1) { Serial.println("Forward + Turning Left"); }
        }
      }
      //if steering left
      else if (steerpos < steer_neutral_lower)
      {
        if (!steering_swap) {
          left_channel_reverse = true;
          if (serial_output == 1) { Serial.println("Forward + Turning Left"); }
        }
        else {
          right_channel_reverse = true;
          if (serial_output == 1) { Serial.println("Forward + Turning Right"); }
        }
      }
      else //if steering is neutral
      {
        if (serial_output == 1) { Serial.println("Forward"); }
      }
      
    }
    //if the throttle is below neutral range (reverse)
    else if (throtpos < throt_neutral_lower)
    {
      
      right_channel = true;
      left_channel = true;
      
      //if steering right
      if (steerpos > steer_neutral_upper)
      {
        if (!steering_reverse_swap) {
          left_channel_reverse = true;
          if (serial_output == 1) { Serial.println("Reversing + Turning Right"); }
        }
        else {
          right_channel_reverse = true;
          if (serial_output == 1) { Serial.println("Reversing + Turning Left"); }
        }
      }
      //if steering left
      else if (steerpos < steer_neutral_lower)
      {
        if (!steering_reverse_swap) {
          right_channel_reverse = true;
          if (serial_output == 1) { Serial.println("Reversing + Turning Left"); }
        }
        else {
          left_channel_reverse = true;
          if (serial_output == 1) { Serial.println("Reversing + Turning Right"); }
        }
      }
      else //if steering is neutral
      {
        left_channel_reverse = true;
        right_channel_reverse = true;
        if (serial_output == 1) { Serial.println("Reversing"); }
      }
      
    }
    else //neutral / idle
    {
      if (serial_output == 1) { Serial.println("Idle"); }
    }
    
  }
  
  //debug output
  if (serial_output == 2) { Serial.println("throttle position: " + String(throtpos) + " steering position: " + String(steerpos)); }
  
  //if the channels are swapped
  if (!channel_swap)
  {
    
    //set the outputs to match the movements calculated above
    if (left_channel) {
      digitalWrite(esc_left_move, HIGH);
    }
    else {
      digitalWrite(esc_left_move, LOW);
    }
    if (right_channel) {
      digitalWrite(esc_right_move, HIGH);
    }
    else {
      digitalWrite(esc_right_move, LOW);
    }
    
    if (!left_channel_reverse) {
      digitalWrite(esc_left_forward, HIGH);
      digitalWrite(esc_left_reverse, LOW);
    }
    else {
      digitalWrite(esc_left_reverse, HIGH);
      digitalWrite(esc_left_forward, LOW);
    }
    
    if (!right_channel_reverse) {
      digitalWrite(esc_right_forward, HIGH);
      digitalWrite(esc_right_reverse, LOW);
    }
    else {
      digitalWrite(esc_right_reverse, HIGH);
      digitalWrite(esc_right_forward, LOW);
    }
  
  }
  else
  {
  
    //set the outputs to match the movements calculated above
    if (right_channel) {
      digitalWrite(esc_left_move, HIGH);
    }
    else {
      digitalWrite(esc_left_move, LOW);
    }
    if (left_channel) {
      digitalWrite(esc_right_move, HIGH);
    }
    else {
      digitalWrite(esc_right_move, LOW);
    }
    
    if (!right_channel_reverse) {
      digitalWrite(esc_left_forward, HIGH);
      digitalWrite(esc_left_reverse, LOW);
    }
    else {
      digitalWrite(esc_left_reverse, HIGH);
      digitalWrite(esc_left_forward, LOW);
    }
    
    if (!left_channel_reverse) {
      digitalWrite(esc_right_forward, HIGH);
      digitalWrite(esc_right_reverse, LOW);
    }
    else {
      digitalWrite(esc_right_reverse, HIGH);
      digitalWrite(esc_right_forward, LOW);
    }
    
  }
  
}

//that's a wrap. go get coffee
