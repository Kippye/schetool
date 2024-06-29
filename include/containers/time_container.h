#pragma once

#include <string>
#include <ctime>

struct TimeContainer
{
    int hours;
    int minutes;

    TimeContainer();
    TimeContainer(int h, int m);
    TimeContainer(const tm& t);
    std::string getString() const;
    int getHours() const;
    int getMinutes() const;
    void setTime(int hours, int minutes);

    TimeContainer& operator+=(const TimeContainer& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        this->minutes += rhs.minutes;
        this->hours += this->minutes / 60;
        this->minutes -= this->minutes / 60 * 60;
        this->hours += rhs.hours;
        this->hours = this->hours > 23 ? this->hours - this->hours / 24 * 24 : this->hours;
        return *this; // return the result by reference
    }

    TimeContainer& operator-=(const TimeContainer& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        this->minutes -= rhs.minutes;
        this->hours = this->minutes < 0 ? this->hours - 1 : this->hours;
        this->minutes = this->minutes < 0 ? 60 + this->minutes : this->minutes;
        this->hours -= rhs.hours;
        this->hours = this->hours < 0 ? 24 + this->hours : this->hours;
        return *this; // return the result by reference
    }
    
    // friends defined inside class body are inline and are hidden from non-ADL lookup
    friend TimeContainer operator+(TimeContainer lhs, const TimeContainer& rhs)
    {
        lhs += rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend TimeContainer operator-(TimeContainer lhs, const TimeContainer& rhs)
    {
        lhs -= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    bool operator==(const TimeContainer& other) const
    {
        return (getHours() == other.getHours() &&
                getMinutes() == other.getMinutes());
    }

    bool operator!=(const TimeContainer& other) const
    {
        return (getHours() != other.getHours() ||
                getMinutes() != other.getMinutes());
    }

    friend bool operator<(const TimeContainer& left, const TimeContainer& right)
    {
        if (left.hours < right.hours) { return true; }
        else if (left.hours == right.hours) 
        {  
            return left.minutes < right.minutes;
        }
        else return false;
    }

    friend bool operator>(const TimeContainer& left, const TimeContainer& right)
    {
        if (left.hours > right.hours) { return true; }
        else if (left.hours == right.hours) 
        {  
            return left.minutes > right.minutes;
        }
        else return false;
    }
};