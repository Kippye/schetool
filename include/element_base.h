#pragma once
#include <time_container.h>
#include <date_container.h>
#include <select_container.h>

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

class ElementBase
{
    protected:
        SCHEDULE_TYPE m_type;
        DateContainer m_creationDate;
        TimeContainer m_creationTime;
        DateContainer m_editDate;
        TimeContainer m_editTime;
    public:
        ElementBase();
        ElementBase(SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime);
        SCHEDULE_TYPE getType() const;
        const DateContainer& getCreationDate() const;
        const TimeContainer& getCreationTime() const;
        const DateContainer& getEditDate() const;
        const TimeContainer& getEditTime() const;

        virtual std::string getString() const;
        virtual ElementBase* getCopy() const;
};