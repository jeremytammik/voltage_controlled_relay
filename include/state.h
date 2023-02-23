// state.h -- state machine

int entry_state(void);
int foo_state(void);
int bar_state(void);
int exit_state(void);

// array and enum below must be in sync! 
int (* state[])(void) = { entry_state, foo_state, bar_state, exit_state};
enum state_codes { entry, foo, bar, end};
