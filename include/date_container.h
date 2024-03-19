#pragma once
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

#include <iostream>

class Date
{
    private:
        tm m_stored_time;
    public:
        Date();
        explicit Date(const tm& time);

        friend bool operator<(const Date& left, const Date& right)
        {
            if (left.m_stored_time.tm_year < right.m_stored_time.tm_year) { return true; }
            else if (left.m_stored_time.tm_year == right.m_stored_time.tm_year)
            {
                if (left.m_stored_time.tm_mon < right.m_stored_time.tm_mon) { return true; }
                else if (left.m_stored_time.tm_mon == right.m_stored_time.tm_mon)
                {
                    return left.m_stored_time.tm_mday < right.m_stored_time.tm_mday;
                }
                else return false;
            }
            else return false;
        }

        friend bool operator>(const Date& left, const Date& right)
        {
            if (left.m_stored_time.tm_year > right.m_stored_time.tm_year) { return true; }
            else if (left.m_stored_time.tm_year == right.m_stored_time.tm_year)
            {
                if (left.m_stored_time.tm_mon > right.m_stored_time.tm_mon) { return true; }
                else if (left.m_stored_time.tm_mon == right.m_stored_time.tm_mon)
                {
                    return left.m_stored_time.tm_mday > right.m_stored_time.tm_mday;
                }
                else return false;
            }
            else return false;
        }

        const tm* getTime();
        std::string getString();
        
        void setTime(const tm& time);
        void setMonthDay(unsigned int day);
        void setMonth(int month);
        void setYear(int year, bool convert);

        static int convertToValidYear(int year, bool hasBeenSubtracted = false, bool subtractTmBaseYear = true);
};