#include <Servo.h>

class ServoController {
  private:
    Servo left_servo;
    Servo right_servo;
    int esc_left;
    int esc_right;
	bool channel_swap;
	int debug_level;
  
  public:
    ServoController(int esc_left, int esc_right, bool channel_swap, int debug_level);
    void attach();
    void move(int left_move, int right_move, int throtpos, int steerpos, char* message, int value);
    void move(int left_move, int right_move, int throtpos, int steerpos);
};
