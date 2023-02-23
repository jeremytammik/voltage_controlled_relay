// state.h -- state machine

int state_all_off(void);
int state_r1_on(void);
int state_r2_on(void);

// array and enum below must be in sync! 
int (* state[])(void) = { state_all_off, state_r1_on, state_r2_on };

enum state_codes { all_off, r1_on, r2_on };

enum ret_codes { ok, voltage_low, voltage_high};

struct transition {
    enum state_codes src_state;
    enum ret_codes   ret_code;
    enum state_codes dst_state;
};

struct transition state_transitions[] = {
    {all_off, ok, all_off},
    {all_off, voltage_low, all_off},
    {all_off, voltage_high, r1_on},
    {r1_on, ok, r1_on},
    {r1_on, voltage_low, all_off},
    {r1_on, voltage_high, r2_on},
    {r2_on, ok, r2_on},
    {r2_on, voltage_low, r1_on},
    {r2_on, voltage_high, r2_on}};
