#pragma once
#include <ctime>
#include <string>

class Time
{
    private:
        tm m_stored_time;
        // TODO: some way to differentiate between ones that show:
        // * the entire date and time
        // * the entire date
        // * the time
        // * the weekday
    public:
        Time();
        explicit Time(const tm& time, bool _displayDate = true, bool _displayTime = true, bool _displayWeekday = false);

        // NOTE: if displayDate, displayTime AND weekday are false, displayDate will be reset to true
        bool displayDate = true;
        bool displayTime = true;
        // if true, displayDate and displayTime are ignored and only the weekday, i.e. "Wednesday", is displayed
        bool displayWeekday = false;

        const tm* getTime();
        std::string getString();
        
        void setTime(const tm& time);
        void setClockTime(unsigned int hour, unsigned int minute, unsigned int second);
        void setMonthDay(unsigned int day);
        void setMonth(unsigned int month);
        void setYear(unsigned int year);
};