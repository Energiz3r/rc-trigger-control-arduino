#include "Receiver.hpp"
#include <Arduino.h>

Receiver::Receiver(float neutral_upper, 
  float neutral_lower, 
  float limit_upper, 
  float limit_lower, 
  int lostcon_upper, 
  int lostcon_lower, 
  bool rate_change_limit, 
  int max_diff,
  int rx_input) {
  
  this->neutral_upper = neutral_upper; 
  this->neutral_lower = neutral_lower; 
  this->limit_upper = limit_upper; 
  this->limit_lower = limit_lower; 
  this->lostcon_upper = lostcon_upper; 
  this->lostcon_lower = lostcon_lower; 
  this->rate_change_limit = rate_change_limit; 
  this->max_diff = max_diff;
  this->rx_input = rx_input;
}

int Receiver::update_position() {
  int pos = pulseIn(rx_input, HIGH, 25000); //read the pulse width of the channel. 25000 is more than sufficient sample size for highly responsive I/O
  if (rate_change_limit) {
    if (pos > last_pos) { 
      if ((pos - last_pos) > max_diff) {
        pos = last_pos + max_diff; 
      } 
    }
    if (pos < last_pos) { 
      if ((last_pos - pos) > max_diff) {
        pos = last_pos - max_diff; 
      } 
    }
  }
  //clamp the values to within the useable range
  if (pos > limit_upper) { 
    pos = limit_upper; 
  } 
  if (pos < limit_lower) { 
    pos = limit_lower; 
  }
  last_pos = pos;
  return pos;
}

float Receiver::upper_percent(int pos) {
  return ((pos - neutral_upper) / (limit_upper - neutral_upper)) * 100;
}

float Receiver::lower_percent(int pos) {
  return ((neutral_lower - pos) / (neutral_lower - limit_lower)) * 100;
}

bool Receiver::is_connection_lost(int pos) {
  return (pos > lostcon_lower) && (pos < lostcon_upper);
}

void Receiver::pin_input() {
  pinMode(rx_input, INPUT);
}

bool Receiver::is_forward(int pos) {
  return pos > neutral_upper;
}

bool Receiver::is_backward(int pos) {
  return pos < neutral_lower;
}

bool Receiver::is_left(int pos) {
  return pos > neutral_upper;
}

bool Receiver::is_right(int pos) {
  return pos < neutral_lower;
}
