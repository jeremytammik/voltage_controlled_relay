/*
    Custom timer class to enable doing delay(Delay) without blocking the program execution

    Usage:
        TimerDelay t;
        t.setDelay(1000);
        t.setSingleShot(true);

        // Or
        TimerDelay t = TimerDelay(1000);
        TimerDelay t = TimerDelay(true, 1000);

        // Start it
        t.start();

        if(t.timedout()) {
            // Do your code here
        }

        // Stop it
        t.stop();
*/
#ifndef TIMERDELAY_H
#define TIMERDELAY_H

class TimerDelay 
{
public:
  TimerDelay();

  TimerDelay(int delay);

  TimerDelay(bool isSingleShot, int delay);

  void start();

  void stop();
  
  void reset();

  void setDelay(int delay);

  void setSingleShot(bool isSingleShot);

  bool timedout();

private:
  unsigned long m_timerStartDelay;      // Time when delay began
  bool m_isRunning = false;             // If timer is currently running
  bool m_isSingleShot = false;          // If it runs only once
  int m_delay = 0;                      // Delay duration in milliseconds
};

#endif  // TIMERDELAY_H