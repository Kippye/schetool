#pragma once
#include <time_container.h>
#include <date_container.h>
#include <schedule_constants.h>

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
        virtual ElementBase* getCopy();
};