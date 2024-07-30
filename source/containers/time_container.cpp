#include <time_container.h>

TimeContainer::TimeContainer() {}
TimeContainer::TimeContainer(int h, int m)
{
    setTime(h, m);
}
TimeContainer::TimeContainer(const tm& t)
{
    setTime(t.tm_hour, t.tm_min);
}

TimeContainer::TimeContainer(const ClockTimeWrapper& clockTime)
{
    setTime(clockTime.getHours(), clockTime.getMinutes());
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
    this->hours = std::min(std::max(hours, 0), 23);
    this->minutes = std::min(std::max(minutes, 0), 59);
}
