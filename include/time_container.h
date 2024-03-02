#pragma once
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

#include <iostream>

class Time
{
    private:
        tm m_stored_time;
        bool m_selectedWeekdays[7] = {false, false, false, false, false, false, false};
    public:
        Time();
        explicit Time(const tm& time);

        Time& operator+=(const Time& rhs) // compound assignment (does not need to be a member,
        {                           // but often is, to modify the private members)
            this->m_stored_time.tm_min += rhs.m_stored_time.tm_min;
            this->m_stored_time.tm_hour += this->m_stored_time.tm_min / 60;
            this->m_stored_time.tm_min -= this->m_stored_time.tm_min / 60 * 60;
            this->m_stored_time.tm_hour += rhs.m_stored_time.tm_hour;
            this->m_stored_time.tm_hour = this->m_stored_time.tm_hour > 23 ? this->m_stored_time.tm_hour - this->m_stored_time.tm_hour / 24 * 24 : this->m_stored_time.tm_hour;
            return *this; // return the result by reference
        }

        Time& operator-=(const Time& rhs) // compound assignment (does not need to be a member,
        {                           // but often is, to modify the private members)
            this->m_stored_time.tm_min -= rhs.m_stored_time.tm_min;
            this->m_stored_time.tm_hour = this->m_stored_time.tm_min < 0 ? this->m_stored_time.tm_hour - 1 : this->m_stored_time.tm_hour;
            this->m_stored_time.tm_min = this->m_stored_time.tm_min < 0 ? 60 + this->m_stored_time.tm_min : this->m_stored_time.tm_min;
            this->m_stored_time.tm_hour -= rhs.m_stored_time.tm_hour;
            this->m_stored_time.tm_hour = this->m_stored_time.tm_hour < 0 ? 24 + this->m_stored_time.tm_hour : this->m_stored_time.tm_hour;
            return *this; // return the result by reference
        }
        
        // friends defined inside class body are inline and are hidden from non-ADL lookup
        friend Time operator+(Time lhs, const Time& rhs)
        {
            lhs += rhs; // reuse compound assignment
            return lhs; // return the result by value (uses move constructor)
        }

        friend Time operator-(Time lhs, const Time& rhs)
        {
            lhs -= rhs; // reuse compound assignment
            return lhs; // return the result by value (uses move constructor)
        }

        const tm* getTime();
        std::string getString(bool displayDate, bool displayTime, bool displayWeekday);
        const bool* getSelectedWeekdays();
        
        void setTime(const tm& time);
        void setClockTime(unsigned int hour, unsigned int minute);
        void setMonthDay(unsigned int day);
        void setMonth(unsigned int month);
        void setYear(unsigned int year);
        void setWeekdaySelected(unsigned int weekday, bool selected);
};