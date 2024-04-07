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
        SCHEDULE_EDIT_TYPE m_type;
    public:
        ScheduleEdit(Schedule* schedule, SCHEDULE_EDIT_TYPE type) 
        { 
            m_schedule = schedule;
            m_type = type;
        }
        virtual ~ScheduleEdit();
        virtual void revert();
        virtual void apply();
        bool getIsReverted() const;
        SCHEDULE_EDIT_TYPE getType() const;
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

        std::pair<size_t, size_t> getPosition() const;
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
        RowEdit(Schedule* schedule, bool isRemove, size_t row, const std::vector<ElementBase*>& elementDataCopy) : ScheduleEdit(schedule, SCHEDULE_EDIT_ROW) 
        {
            m_isRemove = isRemove;
            m_row = row;
            m_elementData = elementDataCopy;
        }

        ~RowEdit() override;

        void revert() override;

        void apply() override;

        bool getIsRemove() const
        {
            return m_isRemove;
        }

        size_t getRow() const
        {
            return m_row;
        }
};

class ColumnEdit : public ScheduleEdit
{
    private:
        bool m_isRemove = false;
        size_t m_column;
        // TODO: clean Element LEAK
        Column* m_columnData;
    public:
        ColumnEdit(Schedule* schedule, bool isRemove, size_t column, const Column& columnData);

        ~ColumnEdit() override;

        void revert() override;

        void apply() override;

        bool getIsRemove()
        {
            return m_isRemove;
        }

        size_t getColumn() const
        {
            return m_column;
        }

        const Column* const getColumnData() const
        {
            return m_columnData;
        }
};