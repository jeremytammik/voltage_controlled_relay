// Check on the header file for documentation
// Created 20/08/2023
// AKK

#include "../include/filter.h"

MovingAverageFilter::MovingAverageFilter() : m_sum(0), m_currentIndex(0)
{
}

float MovingAverageFilter::append(float value)
{
    m_sum -= m_data[m_currentIndex];
    m_data[m_currentIndex] = value;
    m_sum += value;
    m_currentIndex = (m_currentIndex + 1) % WINDOW_SIZE;

    return m_sum / (float)WINDOW_SIZE;
}

float MovingAverageFilter::clear()
{
    int i;

    for (i = 0; i < WINDOW_SIZE; i++)
    {
        m_data[i] = 0;
    }

    m_sum = 0;
    m_currentIndex = 0;
}