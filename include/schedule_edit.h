#pragma once

#include <element.h>
#include <schedule.h>

class Schedule;

enum SCHEDULE_EDIT_TYPE
{
    SCHEDULE_EDIT_ELEMENT
};

class ScheduleEdit
{
    protected:
        Schedule* m_schedule;
        bool m_isReverted = false;
    public:
        SCHEDULE_EDIT_TYPE type;
        ScheduleEdit(Schedule* schedule, SCHEDULE_EDIT_TYPE type) 
        { 
            m_schedule = schedule;
            this->type = type;
        }
        virtual void revert();
        virtual void apply();
        bool getIsReverted() const;
};

class ElementEditBase : public ScheduleEdit
{
    protected:
        SCHEDULE_TYPE m_elementType;
        size_t m_column, m_row;
    public:
        ElementEditBase(Schedule* schedule, size_t column, size_t row, SCHEDULE_TYPE elementType) : ScheduleEdit(schedule, SCHEDULE_EDIT_ELEMENT) 
        {
            m_column = column;
            m_row = row;
            m_elementType = elementType;
        }

        SCHEDULE_TYPE getElementType() const;
};

template <typename T>
class ElementEdit : public ElementEditBase
{
    private:
        T m_previousValue;
        T m_newValue;
    public:
        ElementEdit(Schedule* schedule, size_t column, size_t row, SCHEDULE_TYPE elementType, const T& previousValue, const T& newValue) : ElementEditBase(schedule, column, row, elementType) 
        {
            m_previousValue = previousValue;
            m_newValue = newValue;
        }

        void revert() override
        {
            m_schedule->setElementValue(m_column, m_row, m_previousValue, true, false);
            m_isReverted = true;
        }

        void apply() override
        {
            m_schedule->setElementValue(m_column, m_row, m_newValue, true, false);
            m_isReverted = false;
        }
};