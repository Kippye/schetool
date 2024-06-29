#pragma once

#include <string>
#include <ctime>

struct DateContainer
{
    private:
        tm time;
        bool m_isRelative = false;
        int m_relativeOffset = 0;
    public:
        DateContainer();
        DateContainer(const tm& t, bool isRelative = false, int relativeOffset = 0);
        std::string getString() const;
        tm getTime() const;
        bool getIsRelative() const;
        size_t getRelativeOffset() const;
        void setTime(const tm& time);
        void setMonthDay(unsigned int day);
        void incrementMonthDay();
        void setMonth(int month);
        void incrementMonth();
        void setYear(int year, bool convert);
        void incrementYear();

        static int convertToValidYear(int year, bool hasBeenSubtracted = false, bool subtractTmBaseYear = true);

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