#include "ServoController.hpp"
#include <Arduino.h>

ServoController::ServoController(int esc_left, int esc_right, bool channel_swap, bool steer_forward_swap, bool steer_back_swap, int debug_level) {
  this->esc_left = esc_left;
  this->esc_right = esc_right;
  this->channel_swap = channel_swap;
  this->steer_forward_swap = steer_forward_swap;
  this->steer_back_swap = steer_back_swap;
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

void ServoController::steer_forward_left(int left_move, int right_move, int throtpos, int steerpos, int newoutput) {
  if (steer_forward_swap) {
    move(right_move, left_move, throtpos, steerpos, "(Forward) Left channel servo: ", newoutput);  
  } else {
    move(left_move, right_move, throtpos, steerpos, "(Forward) Left channel servo: ", newoutput);  
  }
}

void ServoController::steer_forward_right(int left_move, int right_move, int throtpos, int steerpos, int newoutput) {
  if (steer_forward_swap) {
    move(right_move, left_move, throtpos, steerpos, "(Forward) Right channel servo: ", newoutput);  
  } else {
    move(left_move, right_move, throtpos, steerpos, "(Forward) Right channel servo: ", newoutput);  
  }
}

void ServoController::steer_backward_left(int left_move, int right_move, int throtpos, int steerpos, int newoutput) {
  if (steer_back_swap) {
    move(right_move, left_move, throtpos, steerpos, "(Reversing) Left channel servo: ", newoutput);  
  } else {
    move(left_move, right_move, throtpos, steerpos, "(Reversing) Left channel servo: ", newoutput);  
  }
}

void ServoController::steer_backward_right(int left_move, int right_move, int throtpos, int steerpos, int newoutput) {
  if (steer_back_swap) {
    move(right_move, left_move, throtpos, steerpos, "(Reversing) Right channel servo: ", newoutput);  
  } else {
    move(left_move, right_move, throtpos, steerpos, "(Reversing) Right channel servo: ", newoutput);  
  }
}
