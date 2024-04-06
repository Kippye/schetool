#pragma once

#include "element_base.h"
#include <element.h>
#include <schedule.h>

class Schedule;
struct Column;

enum SCHEDULE_EDIT_TYPE
{
    SCHEDULE_EDIT_ELEMENT,
    SCHEDULE_EDIT_ROW,
    SCHEDULE_EDIT_COLUMN,
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
        // TODO: clean copy pointers at some point LEAK
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
};

class ColumnEdit : public ScheduleEdit
{
    // here i will need..
    // m_isRemove
    // the column's index
    // maybe i should just make a copy of the ENTIRE Column? i need its properties too.
    // it would also be an easy (if incredibly wasteful) way of making Column property edits, too
    // the entire column's data (easier than for row, just get Column.rows)
    // this is gonna be easy.

    private:
        bool m_isRemove = false;
        size_t m_column;
        // TODO: clean Element LEAK
        Column* m_columnData;
    public:
        ColumnEdit(Schedule* schedule, bool isRemove, size_t column, const Column& columnData);

        void revert() override;

        void apply() override;

        bool getIsRemove()
        {
            return m_isRemove;
        }
};