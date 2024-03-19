#pragma once
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

#include <iostream>

class Time
{
    private:
        int m_hours;
        int m_minutes;
    public:
        Time();
        Time(int hours, int minutes);

        Time& operator+=(const Time& rhs) // compound assignment (does not need to be a member,
        {                           // but often is, to modify the private members)
            this->m_minutes += rhs.m_minutes;
            this->m_hours += this->m_minutes / 60;
            this->m_minutes -= this->m_minutes / 60 * 60;
            this->m_hours += rhs.m_hours;
            this->m_hours = this->m_hours > 23 ? this->m_hours - this->m_hours / 24 * 24 : this->m_hours;
            return *this; // return the result by reference
        }

        Time& operator-=(const Time& rhs) // compound assignment (does not need to be a member,
        {                           // but often is, to modify the private members)
            this->m_minutes -= rhs.m_minutes;
            this->m_hours = this->m_minutes < 0 ? this->m_hours - 1 : this->m_hours;
            this->m_minutes = this->m_minutes < 0 ? 60 + this->m_minutes : this->m_minutes;
            this->m_hours -= rhs.m_hours;
            this->m_hours = this->m_hours < 0 ? 24 + this->m_hours : this->m_hours;
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
            if (left.m_hours < right.m_hours) { return true; }
            else if (left.m_hours == right.m_hours) 
            {  
                return left.m_minutes < right.m_minutes;
            }
            else return false;
        }

        friend bool operator>(const Time& left, const Time& right)
        {
            if (left.m_hours > right.m_hours) { return true; }
            else if (left.m_hours == right.m_hours) 
            {  
                return left.m_minutes > right.m_minutes;
            }
            else return false;
        }

        int getHours();
        int getMinutes();
        std::string getString();
        
        void setTime(unsigned int hour, unsigned int minute);
};