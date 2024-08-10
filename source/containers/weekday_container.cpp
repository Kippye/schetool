#include "weekday_container.h"
#include "time_wrapper.h"

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
    WeekdayContainer currentWeekday;
    currentWeekday.setSelected(TimeWrapper::getCurrentTime().getWeekday(WEEK_START_MONDAY, ZERO_BASED), true);
    return currentWeekday;
}
