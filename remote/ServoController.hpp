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
    float outputcentre;
    float outputmax;
    float outputmin;
	float steering_sensitivity;
    void move(int left_move, int right_move, int throtpos, int steerpos, char* message, int value, bool swap);
    void move(int left_move, int right_move, int throtpos, int steerpos);
    float calc_forward_throt(float throt_percent);
    float calc_backward_throt(float throt_percent);
    float calc_steering_size(float output_throt, float steer_percent);
  
  public:
    ServoController(int esc_left, 
	  int esc_right, 
	  bool channel_swap, 
	  bool steer_forward_swap,
	  bool steer_back_swap, 
      float outputcentre,
      float outputmax,
      float outputmin,
	  float steering_sensitivity,
	  int debug_level);
    void attach();
    void steer_forward(int throtpos, int steerpos, float throt_percent);
    void steer_backward(int throtpos, int steerpos, float throt_percent);
    void steer_idle(int throtpos, int steerpos, bool log_movement);
    void steer_forward_left(int throtpos, int steerpos, float throt_percent, float steer_percent);
    void steer_forward_right(int throtpos, int steerpos, float throt_percent, float steer_percent);
    void steer_backward_left(int throtpos, int steerpos, float throt_percent, float steer_percent);
    void steer_backward_right(int throtpos, int steerpos, float throt_percent, float steer_percent);
};
