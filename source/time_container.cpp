#include <algorithm>
#include <cctype>
#include <ctime>
#include <iterator>
#include <string.h>
#include <time_container.h>
#include <iostream>

Time::Time()
{

}

Time::Time(int hours, int minutes)
{
    setTime(hours, minutes);
}

Time::Time(const TimeContainer& time)
{
    m_time = time;
}

int Time::getHours()
{
    return m_time.hours;
}

int Time::getMinutes()
{
    return m_time.minutes;
}

// Returns the string representation of the stored Time
std::string Time::getString()
{
    try
    {
        return m_time.getString();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        std::cout << "std::string Time::getString(): Failed to convert current time to a string. Invalid time?" << std::endl;
        return std::string("");
    }
}  

// NOTE: Values are automatically clamped below 23 for hours and below 59 for minutes
void Time::setTime(unsigned int hour, unsigned int minute)
{
    m_time.hours = std::min(hour, 23U);
    m_time.minutes = std::min(minute, 59U);
}