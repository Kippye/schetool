#include <ctime>
#include "weekday_container.h"

void WeekdayContainer::update(const SelectOptionChange& change, size_t optionCount)
{
    // do nothing, weekday options can't be changed
}

const std::set<size_t> WeekdayContainer::getSelection() const
{
    return m_selection;
}  

// STATIC
WeekdayContainer WeekdayContainer::getCurrentSystemWeekday()
{
    time_t time = std::time(0);
    tm now = *localtime(&time);
    WeekdayContainer currentWeekday;
    currentWeekday.setSelected(now.tm_wday == 0 ? 6 : now.tm_wday - 1, true);
    return currentWeekday;
}
