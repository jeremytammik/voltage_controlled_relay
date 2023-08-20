#ifndef MOVINGAVERAGEFILTER_H
#define MOVINGAVERAGEFILTER_H

// Defines the moving average filter window size,
// Ajust accordingly
#define WINDOW_SIZE 20

class MovingAverageFilter 
{
    public:
        MovingAverageFilter();

        /**
         * Function takes a single data point read from an analog input
         * or any other source as a parameter and returns an average for the window
        */
        float append(float value);

        /**
         * Function resets the values of the MAF to defaults where necessary
        */
        float clear();

    private:
        int m_sum, m_currentIndex;
        float m_average;
        int m_data[WINDOW_SIZE]={0};
};

#endif // MOVINGAVERAGEFILTER_H