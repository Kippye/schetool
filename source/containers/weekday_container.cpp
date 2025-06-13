#include "weekday_container.h"

void WeekdayContainer::update(const SelectOptionUpdateInfo& change, size_t optionCount) {
    // do nothing, weekday options can't be changed
}

// STATIC
WeekdayContainer WeekdayContainer::getCurrentSystemWeekday(const TimeWrapper& currentTime) {
    WeekdayContainer currentWeekday;
    currentWeekday.setSelected(
        (currentTime.getIsEmpty() ? TimeWrapper::getCurrentTime() : currentTime).getWeekday(WeekStart::Monday, Base::Zero),
        true);
    return currentWeekday;
}
