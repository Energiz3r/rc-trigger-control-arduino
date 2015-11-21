#include "ServoController.hpp"

ServoController::ServoController(int esc_left, int esc_right, bool channel_swap) {
  this->esc_left = esc_left;
  this->esc_right = esc_right;
  this->channel_swap = channel_swap;
}

void ServoController::attach() {
  left_servo.attach(esc_left);
  right_servo.attach(esc_right);
}

void ServoController::move(int left_move, int right_move) {
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
