#pragma once

#include <ctime>

inline const bool ZERO_BASED = 0;
inline const bool ONE_BASED = 1;

class ClockTimeWrapper
{
    private:
        unsigned int m_hours = 0, m_minutes = 0;
    public:
        ClockTimeWrapper();
        ClockTimeWrapper(unsigned int hours, unsigned int minutes);

        unsigned int getHours() const;
        void setHours(unsigned int hours);

        unsigned int getMinutes() const;
        void setMinutes(unsigned int minutes);
};

// A friendlier wrapper for the tm time struct
class TimeWrapper
{
    private:
        unsigned int m_year = 1900, m_month = 1, m_monthDay = 1;
        bool m_empty = true;
        ClockTimeWrapper m_clockTime;
    public:
        TimeWrapper();
        // Construct TimeWrapper and ClockTimeWrapper from tm struct
        TimeWrapper(const tm& t);
        // Construct TimeWrapper from time values
        TimeWrapper(unsigned int year, unsigned int month = 1, unsigned int monthDay = 1);
        // Construct ClockTimeWrapper from copy
        TimeWrapper(const ClockTimeWrapper& clockTime);
        // Construct ClockTimeWrapper from copy and TimeWrapper from values 
        TimeWrapper(const ClockTimeWrapper& clockTime, unsigned int year, unsigned int month = 1, unsigned int monthDay = 1);
  
        ClockTimeWrapper& getClockTime();
        tm getTm() const;
        bool getIsEmpty() const;
        void clear();  
        void setTime(const tm& time);
        // ONE_BASED: Get the month day (1..dayCount).
        // ZERO_BASED: (0..dayCount - 1).
        unsigned int getMonthDay(bool basedness = ONE_BASED) const;
        // ONE_BASED: Set the month day (1..dayCount).
        // ZERO_BASED: (0.. dayCount - 1).
        // NOTE: If the day provided is more than the days in the current month, the day will be set to it
        void setMonthDay(unsigned int day, bool basedness = ONE_BASED);
        // Add 1 to the month day. Handles incrementing months as well.
        void incrementMonthDay();
        // Subtract 1 from the month day. Handles decrementing months as well.
        void decrementMonthDay();

        // ONE_BASED: Get day of the year stored in the time (1..365 or 366).
        // ZERO_BASED: (0..364 or 365).
        unsigned int getYearDay(bool basedness = ONE_BASED) const;
        // ONE_BASED: Sets the month and month day to be this day in the time's year (starting from 1).
        // ZERO_BASED: (starting from 0).
        void setYearDay(unsigned int yearDay, bool basedness = ONE_BASED);

        // ONE_BASED: Get the month (1..12).
        // ZERO_BASED: (0..11).
        unsigned int getMonth(bool basedness = ONE_BASED) const;   
        // ONE_BASED: If the given month is < 1, it will be passed as 12. If it's > 12, it will passed as 1.
        // ZERO_BASED: If the given month is < 0, it will be set to 11. If it's > 11, it will be set to 0.
        void setMonth(int month, bool basedness = ONE_BASED);
        // Add 1 to the month. Handles incrementing year as well.
        void incrementMonth();
        // Subtract 1 from the month. Handles decrementing year as well.
        void decrementMonth();

        // Get the year
        unsigned int getYear() const;
        // Set the year, it will be limited to the range (1900..8000)
        void setYear(unsigned int year);
        void incrementYear();
        void decrementYear();

        // Return the year as years since 1900 (0...)
        static unsigned int toYearsSinceTm(unsigned int year);
        // Convert from the tm struct's years since 1900 to a normal year (i.e. 104 -> 1900 + 104 = 2004)
        static unsigned int fromYearsSinceTm(unsigned int yearsSinceTm);
        // Limit the given year to a range of (1900..8000)
        static unsigned int toValidYear(unsigned int year);

        bool operator==(const TimeWrapper& other) const
        {
            return (m_year == other.m_year &&
                    m_month == other.m_month &&
                    m_monthDay == other.m_monthDay);
        }

        bool operator!=(const TimeWrapper& other) const
        {
            return (m_year != other.m_year ||
                    m_month != other.m_month ||
                    m_monthDay != other.m_monthDay);
        }

        friend bool operator<(const TimeWrapper& left, const TimeWrapper& right)
        {
            if (left.m_year < right.m_year) { return true; }
            else if (left.m_year == right.m_year)
            {
                if (left.m_month < right.m_month) { return true; }
                else if (left.m_month == right.m_month)
                {
                    return left.m_monthDay < right.m_monthDay;
                }
                else return false;
            }
            else return false;
        }

        friend bool operator>(const TimeWrapper& left, const TimeWrapper& right)
        {
            if (left.m_year > right.m_year) { return true; }
            else if (left.m_year == right.m_year)
            {
                if (left.m_month > right.m_month) { return true; }
                else if (left.m_month == right.m_month)
                {
                    return left.m_monthDay > right.m_monthDay;
                }
                else return false;
            }
            else return false;
        }
};