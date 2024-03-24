#pragma once
#include <string>
#include <ctime>

enum SCHEDULE_TYPE
{
    SCH_BOOL,
    SCH_NUMBER,
    SCH_DECIMAL,
    SCH_TEXT,
    SCH_SELECT,
    SCH_TIME,
    SCH_DATE,
    SCH_WEEKDAY,
    SCH_LAST,
};

struct DateContainer
{
    tm time;

    DateContainer();
    DateContainer(const tm& t);
    std::string getString() const;
};

struct TimeContainer
{
    int hours;
    int minutes;

    TimeContainer();
    TimeContainer(int h, int m);
    TimeContainer(const tm& t);
    std::string getString() const;
};

class Element
{ 
    private:
        SCHEDULE_TYPE m_type;
        DateContainer m_creationDate;
        TimeContainer m_creationTime;
        DateContainer m_editDate;
        TimeContainer m_editTime;
    public:
        Element();
        Element(SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime);
        SCHEDULE_TYPE getType() const;
        const DateContainer& getCreationDate() const;
        const TimeContainer& getCreationTime() const;
        const DateContainer& getEditDate() const;
        const TimeContainer& getEditTime() const;
};