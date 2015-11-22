#include "ServoController.hpp"
#include <Arduino.h>

ServoController::ServoController(int esc_left, int esc_right, bool channel_swap, 
    bool steer_forward_swap, bool steer_back_swap, float outputcentre, float outputmax, 
	float outputmin, float steering_sensitivity, int debug_level) {
  this->esc_left = esc_left;
  this->esc_right = esc_right;
  this->channel_swap = channel_swap;
  this->steer_forward_swap = steer_forward_swap;
  this->steer_back_swap = steer_back_swap;
  this->outputcentre = outputcentre;
  this->outputmax = outputmax;
  this->outputmin = outputmin;
  this->steering_sensitivity = steering_sensitivity;
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

void ServoController::move(int left_move, int right_move, int throtpos, int steerpos, char* message, int value, bool swap) {
  //debug output
  if (debug_level == 1) { 
    Serial.println(message + String(value)); 
  }
  if (swap) {
    move(right_move, left_move, throtpos, steerpos);
  } else {
    move(left_move, right_move, throtpos, steerpos);
  }
}

void ServoController::steer_forward(int throtpos, int steerpos, float throt_percent) {
  //find the output throttle value
  float output_throt = ((outputmax - outputcentre) / 100) * throt_percent;
  int output = outputcentre + output_throt;

  move(output, output, throtpos, steerpos, "(Forward) Both servos: ", output, false);  
}

void ServoController::steer_backward(int throtpos, int steerpos, float throt_percent) {
  //find the output throttle value
  float output_throt = ((outputcentre - outputmin) / 100) * throt_percent;
  int output = outputcentre - output_throt;
		
  move(output, output, throtpos, steerpos, "(Reversing) Both servos: ", output, false);
}

void ServoController::steer_idle(int throtpos, int steerpos, bool log_movement) {
  if (log_movement) {
    move(outputcentre, outputcentre, throtpos, steerpos, "(Neutral) Both servos: ", outputcentre, false);  
  } else {
    move(outputcentre, outputcentre, throtpos, steerpos);  
  }
}

void ServoController::steer_forward_left(int throtpos, int steerpos, float throt_percent, float steer_percent) {
  //find the output throttle value
  float output_throt = ((outputmax - outputcentre) / 100) * throt_percent;

  //take that percentage, and subtract it from the throttle output for the left channel, multiplied by sensitivity so it will go in reverse at full steer
  steer_percent = outputcentre + output_throt - (steering_sensitivity * ((output_throt / 100) * steer_percent));
  int newoutput = steer_percent; //convert to int
  int regoutput = outputcentre + output_throt;

  move(regoutput, newoutput, throtpos, steerpos, "(Forward) Left channel servo: ", newoutput, steer_forward_swap);
}

void ServoController::steer_forward_right(int throtpos, int steerpos, float throt_percent, float steer_percent) {
  //find the output throttle value
  float output_throt = ((outputmax - outputcentre) / 100) * throt_percent;
        
  //take that percentage, and subtract it from the throttle output for the right channel, multiplied by sensitivity so it will go in reverse at full steer
  steer_percent = outputcentre + output_throt - (steering_sensitivity * ((output_throt / 100) * steer_percent));
  int newoutput = steer_percent; //convert to int
  int regoutput = outputcentre + output_throt;

  move(newoutput, regoutput, throtpos, steerpos, "(Forward) Right channel servo: ", newoutput, steer_forward_swap);
}

void ServoController::steer_backward_left(int throtpos, int steerpos, float throt_percent, float steer_percent) {
  //find the output throttle value
  float output_throt = ((outputcentre - outputmin) / 100) * throt_percent;
        
  //take that percentage, and subtract it from the throttle output for the left channel, multiplied by sensitivity so it will go in reverse at full steer
  steer_percent = outputcentre - output_throt + (steering_sensitivity * ((output_throt / 100) * steer_percent));
  int newoutput = steer_percent; //convert to int
  int regoutput = outputcentre - output_throt;

  move(regoutput, newoutput, throtpos, steerpos, "(Reversing) Left channel servo: ", newoutput, steer_back_swap);
}

void ServoController::steer_backward_right(int throtpos, int steerpos, float throt_percent, float steer_percent) {
  //find the output throttle value
  float output_throt = ((outputcentre - outputmin) / 100) * throt_percent;
        
  //take that percentage, and subtract it from the throttle output for the right channel, multiplied by sensitivity so it will go in reverse at full steer
  steer_percent = outputcentre - output_throt + (steering_sensitivity * ((output_throt / 100) * steer_percent));
  int newoutput = steer_percent; //convert to int
  int regoutput = outputcentre - output_throt;

  move(newoutput, regoutput, throtpos, steerpos, "(Reversing) Right channel servo: ", newoutput, steer_back_swap);
}
