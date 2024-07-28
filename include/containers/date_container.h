#pragma once

#include <string>
#include <ctime>

struct DateContainer
{
    private:
        tm time;
        bool m_empty = true;
    public:
        DateContainer();
        DateContainer(const tm& t);
        std::string getString(bool asRelative = false) const;
        tm getTime() const;
        bool getIsEmpty() const;
        void clear();  
        void setTime(const tm& time);
        unsigned int getMonthDay() const;
        // NOTE: If the day provided is more than the days in the current month, the day will be clamped to it
        void setMonthDay(unsigned int day);
        // Add 1 to the month day. Handles incrementing months as well.
        void incrementMonthDay();
        // Subtract 1 from the month day. Handles decrementing months as well.
        void decrementMonthDay();
        unsigned int getMonth() const;
        // NOTE: If the given month is < 0, it will be set to 11. If it's > 11, it will be set to 0
        void setMonth(int month);
        // Add 1 to the month. Handles incrementing year as well.
        void incrementMonth();
        // Subtract 1 from the month. Handles decrementing year as well.
        void decrementMonth();
        unsigned int getYear(bool asFull) const;
        void setYear(int year, bool convert);
        void incrementYear();
        void decrementYear();
        // Returns (this - other) but in month days between the dates.
        // NOTE: Mostly meant to work for a few days, might be more of an approximation over multiple years.
        int getDayDifference(const DateContainer& other) const;

        static int convertToValidYear(int year, bool hasBeenSubtracted = false, bool subtractTmBaseYear = true);

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
            return (getTime().tm_year == other.getTime().tm_year &&
                    getTime().tm_mon == other.getTime().tm_mon &&
                    getTime().tm_mday == other.getTime().tm_mday);
        }

        bool operator!=(const DateContainer& other) const
        {
            return (getTime().tm_year != other.getTime().tm_year ||
                    getTime().tm_mon != other.getTime().tm_mon ||
                    getTime().tm_mday != other.getTime().tm_mday);
        }

        friend bool operator<(const DateContainer& left, const DateContainer& right)
        {
            if (left.getTime().tm_year < right.getTime().tm_year) { return true; }
            else if (left.getTime().tm_year == right.getTime().tm_year)
            {
                if (left.getTime().tm_mon < right.getTime().tm_mon) { return true; }
                else if (left.getTime().tm_mon == right.getTime().tm_mon)
                {
                    return left.getTime().tm_mday < right.getTime().tm_mday;
                }
                else return false;
            }
            else return false;
        }

        friend bool operator>(const DateContainer& left, const DateContainer& right)
        {
            if (left.getTime().tm_year > right.getTime().tm_year) { return true; }
            else if (left.getTime().tm_year == right.getTime().tm_year)
            {
                if (left.getTime().tm_mon > right.getTime().tm_mon) { return true; }
                else if (left.getTime().tm_mon == right.getTime().tm_mon)
                {
                    return left.getTime().tm_mday > right.getTime().tm_mday;
                }
                else return false;
            }
            else return false;
        }

        static DateContainer getCurrentSystemDate();
};