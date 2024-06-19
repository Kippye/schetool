#include <ctime>
#include "weekday_container.h"

void WeekdayContainer::update(const SelectOptionChange& change, size_t optionCount)
{
    // do nothing, weekday options can't be changed
}

bool WeekdayContainer::getIsToday() const
{
    return m_isToday;
}

void WeekdayContainer::setIsToday(bool isToday)
{
    m_isToday = isToday;
}

const std::set<size_t> WeekdayContainer::getSelection() const
{
    if (m_isToday == false)
    {
        return m_selection;
    }
    else
    {
        return getCurrentSystemWeekday().getSelection();
    }
}  

// STATIC
WeekdayContainer WeekdayContainer::getCurrentSystemWeekday()
{
    tm now;
    time_t time = std::time(0);
    localtime_s(&now, &time);
    WeekdayContainer currentWeekday;
    currentWeekday.setSelected(now.tm_wday == 0 ? 6 : now.tm_wday - 1, true);
    return currentWeekday;
}