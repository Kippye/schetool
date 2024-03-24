#pragma once
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <element.h>
#include <iostream>

class Time : public Element
{
    private:
        TimeContainer m_time;
    public:
        Time();
        Time(int hours, int minutes);
        Time(const TimeContainer& time);
        Time(int hours, int minutes, SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime) : Element(type, creationDate, creationTime) 
        {
            setTime(hours, minutes);
        }
        Time(SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime) : Element(type, creationDate, creationTime) {}

        Time& operator+=(const Time& rhs) // compound assignment (does not need to be a member,
        {                           // but often is, to modify the private members)
            this->m_time.minutes += rhs.m_time.minutes;
            this->m_time.hours += this->m_time.minutes / 60;
            this->m_time.minutes -= this->m_time.minutes / 60 * 60;
            this->m_time.hours += rhs.m_time.hours;
            this->m_time.hours = this->m_time.hours > 23 ? this->m_time.hours - this->m_time.hours / 24 * 24 : this->m_time.hours;
            return *this; // return the result by reference
        }

        Time& operator-=(const Time& rhs) // compound assignment (does not need to be a member,
        {                           // but often is, to modify the private members)
            this->m_time.minutes -= rhs.m_time.minutes;
            this->m_time.hours = this->m_time.minutes < 0 ? this->m_time.hours - 1 : this->m_time.hours;
            this->m_time.minutes = this->m_time.minutes < 0 ? 60 + this->m_time.minutes : this->m_time.minutes;
            this->m_time.hours -= rhs.m_time.hours;
            this->m_time.hours = this->m_time.hours < 0 ? 24 + this->m_time.hours : this->m_time.hours;
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

        friend bool operator<(const Time& left, const Time& right)
        {
            if (left.m_time.hours < right.m_time.hours) { return true; }
            else if (left.m_time.hours == right.m_time.hours) 
            {  
                return left.m_time.minutes < right.m_time.minutes;
            }
            else return false;
        }

        friend bool operator>(const Time& left, const Time& right)
        {
            if (left.m_time.hours > right.m_time.hours) { return true; }
            else if (left.m_time.hours == right.m_time.hours) 
            {  
                return left.m_time.minutes > right.m_time.minutes;
            }
            else return false;
        }

        int getHours();
        int getMinutes();
        std::string getString();
        
        void setTime(unsigned int hour, unsigned int minute);
};