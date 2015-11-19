// Defines the configuration that can be supplied for a receiver
class Receiver {
  // Member variables, will be private when functions moved into this class
  public:
    float neutral_upper;
    float neutral_lower;
    float limit_upper;
    float limit_lower;
    int lostcon_upper;
    int lostcon_lower;
    int rx_input;
    bool rate_change_limit;

    // Member functions
	  int pulse_in_and_limit(int last_pos, int max_diff);
};

