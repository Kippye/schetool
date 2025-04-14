#include "weekday_container.h"

void WeekdayContainer::update(const SelectOptionUpdateInfo& change, size_t optionCount) {
    // do nothing, weekday options can't be changed
}

// STATIC
WeekdayContainer WeekdayContainer::getCurrentSystemWeekday(const TimeWrapper& currentTime) {
    WeekdayContainer currentWeekday;
    currentWeekday.setSelected(
        (currentTime.getIsEmpty() ? TimeWrapper::getCurrentTime() : currentTime).getWeekday(WEEK_START_MONDAY, ZERO_BASED),
        true);
    return currentWeekday;
}
