#include "Receiver.hpp"
#include <Arduino.h>

int Receiver::pulse_in_and_limit(int last_pos, int max_diff) {
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
