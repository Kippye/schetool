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
    m_hours = hours;
    m_minutes = minutes;
}

int Time::getHours()
{
    return m_hours;
}

int Time::getMinutes()
{
    return m_minutes;
}

// Returns the string representation of the stored time, depending on the displayDate, displayTime and displayWeekday values.
std::string Time::getString()
{
    try
    {
        char output[1024];

        tm outputTime;
        outputTime.tm_hour = m_hours;
        outputTime.tm_min = m_minutes;
        std::strftime(output, sizeof(output), "%H:%M", &outputTime);

        return std::string(output);
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
    m_hours = std::min(hour, 23U);
    m_minutes = std::min(minute, 59U);
}