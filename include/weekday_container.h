#pragma once

#include <string>
#include "select_container.h"

// A select container that holds one or up to 7 of the weekdays
struct WeekdayContainer : public SelectContainer
{
    private:
        size_t m_optionCount = 0;
        bool m_isToday = false;
    public:
        bool operator==(const WeekdayContainer& other)
        {
            if (m_isToday == false)
            {
                return m_selection == other.getSelection();
            }
            else
            {
                size_t currentWeekdayIndex = *getCurrentSystemWeekday().getSelection().begin();
                std::set<size_t> otherSelection = other.getSelection();
                return otherSelection.find(currentWeekdayIndex) != otherSelection.end();
            }
        }

        bool operator!=(const WeekdayContainer& other)
        {
            if (m_isToday == false)
            {
                return m_selection != other.getSelection();
            }
            else
            {
                size_t currentWeekdayIndex = *getCurrentSystemWeekday().getSelection().begin();
                std::set<size_t> otherSelection = other.getSelection();
                return otherSelection.find(currentWeekdayIndex) == otherSelection.end();
            }        
        }

        void update(const SelectOptionChange& change, size_t optionCount);
        const std::set<size_t> getSelection() const;
        // Check if this WeekdayContainer behaves as the current system weekday (compares with getCurrentSystemWeekday instead of itself)
        bool getIsToday() const;
        void setIsToday(bool isToday);

        static WeekdayContainer getCurrentSystemWeekday();
};