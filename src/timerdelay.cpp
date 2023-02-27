#include "timerdelay.h"
#include <Arduino.h>

TimerDelay::TimerDelay()
{
}

TimerDelay::TimerDelay(int delay)
    : m_delay(delay)
{
}

TimerDelay::TimerDelay(bool isSingleShot, int delay)
    : m_isSingleShot(isSingleShot),
      m_delay(delay)
{
}

void TimerDelay::start()
{
    m_isRunning = true;
    m_timerStartDelay = millis();
    Serial.println("Starting timer ...");
}

void TimerDelay::stop()
{
    m_isRunning = false;
}

void TimerDelay::reset()
{
    m_timerStartDelay = millis();
}

void TimerDelay::setDelay(int delay)
{
    m_delay = delay;
}

void TimerDelay::setSingleShot(bool isSingleShot)
{
    m_isSingleShot = isSingleShot;
}

bool TimerDelay::timedout()
{
    if (m_isRunning && ((millis() - m_timerStartDelay) >= m_delay))
    {
        if (m_isSingleShot)
        {
            stop();
        }

        else
        {
            m_timerStartDelay = millis();
        }

        return true;
    }

    return false;
}
