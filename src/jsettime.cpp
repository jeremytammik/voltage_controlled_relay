#include <time.h>
#include <sys/time.h>
#include "jsettime.h"
#include "util.h"

void jsettime(int year, int month, int day, int hour, int minute, int second )
{
    struct tm tm;
    tm.tm_year = year - 1900;
    tm.tm_mon = month;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    time_t t = mktime(&tm);
    Serialprintln("Setting time: %s", asctime(&tm));
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);
}  
