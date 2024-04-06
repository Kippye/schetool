#pragma once

#include "element_base.h"
#include <element.h>
#include <schedule.h>

class Schedule;

enum SCHEDULE_EDIT_TYPE
{
    SCHEDULE_EDIT_ELEMENT,
    SCHEDULE_EDIT_ROW,
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

class RowEdit : public ScheduleEdit
{
    private:
        bool m_isRemove = false;
        size_t m_row;
        // TODO: clean copy pointers at some point
        std::vector<ElementBase*> m_elementData = {};
    public:
        RowEdit(Schedule* schedule, bool isRemove, size_t row, const std::vector<ElementBase*>& elementData) : ScheduleEdit(schedule, SCHEDULE_EDIT_ROW) 
        {
            m_isRemove = isRemove;
            m_row = row;
            m_elementData = elementData;
        }

        void revert() override;

        void apply() override;

        bool getIsRemove()
        {
            return m_isRemove;
        }
    // here i will need..
    // well, first i need to think if i want to mix Row Add and Remove into one type?
    // a row Add would need:
    // basically just the index of the row. then revert will just remove it again. but how to support redo for that? i guess i'd still need a copy of the row's contents
    // a row Remove would need:
    // the index the row was removed at
    // the removed row's contents so it can be readded with revert (schedule->addrow and set every column's element value OR add a special function to schedule)
    // OK so both of them need an index and row data. i can mix them together.
};