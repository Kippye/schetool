#include <time_container.h>

TimeContainer::TimeContainer() {}
TimeContainer::TimeContainer(int h, int m)
{
    hours = h;
    minutes = m;
}
TimeContainer::TimeContainer(const tm& t)
{
    hours = t.tm_hour;
    minutes = t.tm_min; 
}

std::string TimeContainer::getString() const
{
    char output[1024];

    tm outputTime;
    outputTime.tm_hour = hours;
    outputTime.tm_min = minutes;
    std::strftime(output, sizeof(output), "%H:%M", &outputTime);

    return std::string(output);
}

int TimeContainer::getHours() const 
{
    return hours;
}

int TimeContainer::getMinutes() const
{
    return minutes;
}

void TimeContainer::setTime(int hours, int minutes)
{
    this->hours = hours;
    this->minutes = minutes;
}
