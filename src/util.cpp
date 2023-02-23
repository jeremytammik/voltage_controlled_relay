// util.cpp -- utility functions

#include <stdarg.h>
#include <Arduino.h>
#include "util.h"

// Serialprintln - stdarg variable argument printing utility function
// https://arduino.stackexchange.com/questions/56517/formatting-strings-in-arduino-for-output
// src/util.cpp:21:43: warning: 'float' is promoted to 'double' when passed through '...'
// src/util.cpp:21:43: note: (so you should pass 'double' not 'float' to 'va_arg')
// src/util.cpp:21:43: note: if this code is reached, the program will abort
void Serialprintln(const char* input...) {
  va_list args;
  va_start(args, input);
  for(const char* i=input; *i!=0; ++i) {
    if(*i!='%') { Serial.print(*i); continue; }
    switch(*(++i)) {
      case '%': Serial.print('%'); break;
      case 's': Serial.print(va_arg(args, char*)); break;
      case 'd': Serial.print(va_arg(args, int), DEC); break;
      case 'b': Serial.print(va_arg(args, int), BIN); break;
      case 'o': Serial.print(va_arg(args, int), OCT); break;
      case 'x': Serial.print(va_arg(args, int), HEX); break;
      case 'f': Serial.print(va_arg(args, double), 2); break;
    }
  }
  Serial.println();
  va_end(args);
}
