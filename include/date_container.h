#pragma once

#include <string>
#include <ctime>

struct DateContainer
{
    private:
        tm time;
    public:
        DateContainer();
        DateContainer(const tm& t);
        std::string getString() const;
        const tm* getTime() const;
        void setTime(const tm& time);
        void setMonthDay(unsigned int day);
        void setMonth(int month);
        void setYear(int year, bool convert);

        static int convertToValidYear(int year, bool hasBeenSubtracted = false, bool subtractTmBaseYear = true);

        bool operator==(const DateContainer& other) const
        {
            return (time.tm_year == other.getTime()->tm_year &&
                    time.tm_mon == other.getTime()->tm_mon &&
                    time.tm_mday == other.getTime()->tm_mday);
        }

        bool operator!=(const DateContainer& other) const
        {
            return (time.tm_year != other.getTime()->tm_year ||
                time.tm_mon != other.getTime()->tm_mon ||
                time.tm_mday != other.getTime()->tm_mday);
        }

        friend bool operator<(const DateContainer& left, const DateContainer& right)
        {
            if (left.time.tm_year < right.time.tm_year) { return true; }
            else if (left.time.tm_year == right.time.tm_year)
            {
                if (left.time.tm_mon < right.time.tm_mon) { return true; }
                else if (left.time.tm_mon == right.time.tm_mon)
                {
                    return left.time.tm_mday < right.time.tm_mday;
                }
                else return false;
            }
            else return false;
        }

        friend bool operator>(const DateContainer& left, const DateContainer& right)
        {
            if (left.time.tm_year > right.time.tm_year) { return true; }
            else if (left.time.tm_year == right.time.tm_year)
            {
                if (left.time.tm_mon > right.time.tm_mon) { return true; }
                else if (left.time.tm_mon == right.time.tm_mon)
                {
                    return left.time.tm_mday > right.time.tm_mday;
                }
                else return false;
            }
            else return false;
        }
};