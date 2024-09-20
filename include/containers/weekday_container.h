#pragma once

#include <string>
#include "select_container.h"
#include "time_wrapper.h"

// A select container that holds one or up to 7 of the weekdays
struct WeekdayContainer : public SelectContainer
{
    private:
        size_t m_optionCount = 0;
    public:
        bool operator==(const WeekdayContainer& other)
        {
            return m_selection == other.getSelection();
        }

        bool operator!=(const WeekdayContainer& other)
        {
            return m_selection != other.getSelection();    
        }

        void update(const SelectOptionUpdateInfo& change, size_t optionCount);
        const std::set<size_t> getSelection() const;

        static WeekdayContainer getCurrentSystemWeekday(const TimeWrapper& currentTime = TimeWrapper::getCurrentTime());
};