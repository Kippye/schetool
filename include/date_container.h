#pragma once
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <element.h>

#include <iostream>

class Date : public Element
{
    private:
        DateContainer m_date;
    public:
        Date();
        explicit Date(const tm& time);
        Date(const DateContainer& m_date);
        Date(SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime) : Element(type, creationDate, creationTime) {}
        Date(const tm& time, SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime) : Element(type, creationDate, creationTime) 
        {
            setTime(time);
        }

        friend bool operator<(const Date& left, const Date& right)
        {
            if (left.m_date.time.tm_year < right.m_date.time.tm_year) { return true; }
            else if (left.m_date.time.tm_year == right.m_date.time.tm_year)
            {
                if (left.m_date.time.tm_mon < right.m_date.time.tm_mon) { return true; }
                else if (left.m_date.time.tm_mon == right.m_date.time.tm_mon)
                {
                    return left.m_date.time.tm_mday < right.m_date.time.tm_mday;
                }
                else return false;
            }
            else return false;
        }

        friend bool operator>(const Date& left, const Date& right)
        {
            if (left.m_date.time.tm_year > right.m_date.time.tm_year) { return true; }
            else if (left.m_date.time.tm_year == right.m_date.time.tm_year)
            {
                if (left.m_date.time.tm_mon > right.m_date.time.tm_mon) { return true; }
                else if (left.m_date.time.tm_mon == right.m_date.time.tm_mon)
                {
                    return left.m_date.time.tm_mday > right.m_date.time.tm_mday;
                }
                else return false;
            }
            else return false;
        }

        const tm* getTime() const;
        std::string getString() const;
        
        void setTime(const tm& time);
        void setMonthDay(unsigned int day);
        void setMonth(int month);
        void setYear(int year, bool convert);

        static int convertToValidYear(int year, bool hasBeenSubtracted = false, bool subtractTmBaseYear = true);
};