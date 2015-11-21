#include "ServoController.hpp"
#include <Arduino.h>

ServoController::ServoController(int esc_left, int esc_right, bool channel_swap, int debug_level) {
  this->esc_left = esc_left;
  this->esc_right = esc_right;
  this->channel_swap = channel_swap;
  this->debug_level = debug_level;
}

void ServoController::attach() {
  left_servo.attach(esc_left);
  right_servo.attach(esc_right);
}

void ServoController::move(int left_move, int right_move, int throtpos, int steerpos) {  
  //debug output
  if (debug_level == 2) { 
    Serial.println("throttle position: " + String(throtpos) + " steering position: " + String(steerpos)); 
  }

  //if the channels are swapped
  if (channel_swap) {
    //set the outputs to match the calculated movements
    left_servo.write(right_move);
    right_servo.write(left_move);
  } else {
    //set the outputs to match the calculated movements
    left_servo.write(left_move);
    right_servo.write(right_move);
  }
}

void ServoController::move(int left_move, int right_move, int throtpos, int steerpos, char* message, int value) {
  //debug output
  if (debug_level == 1) { 
    Serial.println(message + String(value)); 
  }
  move(left_move, right_move, throtpos, steerpos);
}
