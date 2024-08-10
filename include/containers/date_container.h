#pragma once

#include <string>
#include "time_wrapper.h"

struct DateContainer
{
    private:
        TimeWrapper m_time;
    public:
        DateContainer();
        DateContainer(const TimeWrapper& time);

        std::string getString(bool asRelative = false) const;

        bool getIsEmpty() const;
        void clear();

        TimeWrapper& getTime();
        const TimeWrapper& getTimeConst() const;
        void setTime(const TimeWrapper& time);

        // Add 1 to the month day. Handles incrementing months as well.
        void incrementMonthDay();
        // Subtract 1 from the month day. Handles decrementing months as well.
        void decrementMonthDay();
        
        // Add 1 to the month. Handles incrementing year as well.
        void incrementMonth();
        // Subtract 1 from the month. Handles decrementing year as well.
        void decrementMonth();

        void incrementYear();
        void decrementYear();

        // Returns the difference in month days between the dates (this - other).
        int getDayDifference(const DateContainer& other) const;

        // Prefix increment. Increment month day by +1
        DateContainer& operator++()
        {
            incrementMonthDay();
            return *this; // return new value by reference
        }
    
        // Postfix increment. Increment month day by +1
        DateContainer operator++(int)
        {
            DateContainer old = *this; // copy old value
            operator++();  // prefix increment
            return old;    // return old value
        }
    
        // Prefix decrement. Reduce month day by 1
        DateContainer& operator--()
        {
            decrementMonthDay();
            return *this; // return new value by reference
        }
    
        // Prefix decrement. Reduce month day by 1
        DateContainer operator--(int)
        {
            DateContainer old = *this; // copy old value
            operator--();  // prefix decrement
            return old;    // return old value
        }

        bool operator==(const DateContainer& other) const
        {
            return getTimeConst().getDateUTC() == other.getTimeConst().getDateUTC();
        }

        bool operator!=(const DateContainer& other) const
        {
            return getTimeConst().getDateUTC() != other.getTimeConst().getDateUTC();
        }

        friend bool operator<(const DateContainer& left, const DateContainer& right)
        {
            return left.getTimeConst().getDateUTC() < right.getTimeConst().getDateUTC();
        }

        friend bool operator>(const DateContainer& left, const DateContainer& right)
        {
            return left.getTimeConst().getDateUTC() > right.getTimeConst().getDateUTC();
        }

        static DateContainer getCurrentSystemDate();
};