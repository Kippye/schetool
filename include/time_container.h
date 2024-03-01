#pragma once
#include <ctime>
#include <string>

class Time
{
    private:
        tm m_stored_time;
        bool m_selectedWeekdays[7] = {false, false, false, false, false, false, false};
    public:
        Time();
        explicit Time(const tm& time);

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