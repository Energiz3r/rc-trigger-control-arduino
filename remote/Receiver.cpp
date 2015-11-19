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