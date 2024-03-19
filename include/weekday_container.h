#pragma once
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

#include <iostream>

class Weekday
{
    private:
        bool m_selectedWeekdays[7] = {false, false, false, false, false, false, false};
    public:
        Weekday();

        friend bool operator<(const Weekday& left, const Weekday& right)
        {
            return true; // TODO!
        }

        friend bool operator>(const Weekday& left, const Weekday& right)
        {
            return true; // TODO!
        }

        std::string getString();
        const bool* getSelectedWeekdays();
        
        void setWeekdaySelected(unsigned int weekday, bool selected);
};