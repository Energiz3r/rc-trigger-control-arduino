//--------------------------------------------------------------
//  This script lets you use a trigger-style R/C controller
//    to control a tank-style bot with left/right drivers
//--------------------------------------------------------------
// v1.0
//TO DO:
// -Add support for per-channel upper/lower/centre limits (useful if ESCs or motors are not identical)

#include "config.h"

int long tracklostcon = 0; //tracks if the connection is interrupted

void setup() {
  //set up pins
  servos.attach();
  throttle.pin_input(); //throttle input
  steer.pin_input(); //steering input

  Serial.begin(9600); //pour a bowl of Serial
}

bool is_connected(int throtpos, int steerpos) {
  bool connection_ok = true;
  
  //if the conditions for 'connection lost' are met, ie. the outputs from the receiver are within a specific range
  if (throttle.is_connection_lost(throtpos) && steer.is_connection_lost(steerpos)) {
    //keep track of how long the connection lost conditions have been met
    if (tracklostcon == 0) {
      tracklostcon = millis();
      
      //debug output
      if (serial_output == 1) { 
        Serial.println("Connection seems to be interrupted... "); 
      }
    }
    
    //if the connection is still interrupted after the allowed time the TX is probably off or out of range
    if ((millis() - tracklostcon) > lostconwaittime) {
      //disable normal movement
      connection_ok = false;
      
      //debug output
      if (serial_output == 1) { 
        Serial.println("Connection lost!"); 
      }
    }
  } else {
    //reset lost connection tracking if conditions are normal to prevent stalling unnecessarily
    tracklostcon = 0;
  }
  return connection_ok;
}

void loop() {
  delay(10);
  //stores the current positions for the steering and throttle PWM values
  const int throtpos = throttle.update_position();
  const int steerpos = steer.update_position();
  
  //only perform normal movements if the lost connection timer hasn't been exceeded
  if (!check_lost_con || is_connected(throtpos, steerpos)) {
    //if the throttle position is above the neutral range (forward)
    if (throttle.is_forward(throtpos)) {
      //if steering left
      if (steer.is_left(steerpos)) {
        servos.steer_forward_left(throtpos, steerpos, throttle.upper_percent(throtpos), steer.upper_percent(steerpos));
      } else if (steer.is_right(steerpos)) { //if steering right
        servos.steer_forward_right(throtpos, steerpos, throttle.upper_percent(throtpos), steer.lower_percent(steerpos));
      } else { //if steering is neutral
        servos.steer_forward(throtpos, steerpos, throttle.upper_percent(throtpos));
      }
    } else if (throttle.is_backward(throtpos)) { //if the throttle is below neutral range (reverse)
      //if steering left
      if (steer.is_left(steerpos)) {
        servos.steer_backward_left(throtpos, steerpos, throttle.lower_percent(throtpos), steer.upper_percent(steerpos));
      } else if (steer.is_right(steerpos)) { //if steering right
        servos.steer_backward_right(throtpos, steerpos, throttle.lower_percent(throtpos), steer.lower_percent(steerpos));
      } else { //if steering is neutral
        servos.steer_backward(throtpos, steerpos, throttle.lower_percent(throtpos));
      }
    } else { //neutral / idle
      servos.steer_idle(throtpos, steerpos, true);
    }
  } else {
    //set outputs to neutral positions
    servos.steer_idle(throtpos, steerpos, false);
  }
}

//that's a wrap. go get coffee
