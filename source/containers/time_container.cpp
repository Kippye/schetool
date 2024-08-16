#include "time_container.h"
#include "time_wrapper.h"

TimeContainer::TimeContainer() {}
TimeContainer::TimeContainer(int h, int m)
{
    setTime(h, m);
}

TimeContainer::TimeContainer(const ClockTimeWrapper& clockTime)
{
    setTime(clockTime.getHours(), clockTime.getMinutes());
}

std::string TimeContainer::getString() const
{
    return TimeWrapper(ClockTimeWrapper(hours, minutes)).getStringUTC(TIME_FORMAT_TIME);
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
