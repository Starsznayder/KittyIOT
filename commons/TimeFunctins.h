#ifndef TIMEFUNCTIONS_H
#define TIMEFUNCTIONS_H

#include <chrono>

namespace kitty{

int timestamp2day(uint64_t timestamp)
{
    std::chrono::system_clock::time_point uptime_timepoint{std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(std::chrono::microseconds(timestamp))};
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(uptime_timepoint);
    struct tm ts = *localtime(&unix_timestamp);
    return ts.tm_mday;
}

int timestamp2hour(uint64_t timestamp)
{
    std::chrono::system_clock::time_point uptime_timepoint{std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(std::chrono::microseconds(timestamp))};
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(uptime_timepoint);
    struct tm ts = *localtime(&unix_timestamp);
    return ts.tm_hour;
}

int timestamp2minute(uint64_t timestamp)
{
    std::chrono::system_clock::time_point uptime_timepoint{std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(std::chrono::microseconds(timestamp))};
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(uptime_timepoint);
    struct tm ts = *localtime(&unix_timestamp);
    return ts.tm_min;
}

}


#endif // TIMEFUNCTIONS_H
