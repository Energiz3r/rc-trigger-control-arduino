#include <Servo.h>

class ServoController {
  private:
    Servo left_servo;
    Servo right_servo;
    int esc_left;
    int esc_right;
	bool channel_swap;
	bool steer_forward_swap;
	bool steer_back_swap;
	int debug_level;
    void move(int left_move, int right_move, int throtpos, int steerpos, char* message, int value, bool swap);
    void move(int left_move, int right_move, int throtpos, int steerpos);
  
  public:
    ServoController(int esc_left, int esc_right, bool channel_swap, bool steer_forward_swap, bool steer_back_swap, int debug_level);
    void attach();
    void steer_forward(int forward_move, int throtpos, int steerpos);
    void steer_backward(int backward_move, int throtpos, int steerpos);
    void steer_idle(int centre, int throtpos, int steerpos, bool log_movement);
    void steer_forward_left(int left_move, int right_move, int throtpos, int steerpos, int newoutput);
    void steer_forward_right(int left_move, int right_move, int throtpos, int steerpos, int newoutput);
    void steer_backward_left(int left_move, int right_move, int throtpos, int steerpos, int newoutput);
    void steer_backward_right(int left_move, int right_move, int throtpos, int steerpos, int newoutput);
};
