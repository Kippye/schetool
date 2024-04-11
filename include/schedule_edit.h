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
    SCHEDULE_EDIT_COLUMN_PROPERTY
};

enum COLUMN_PROPERTY
{
    COLUMN_PROPERTY_NAME,
    COLUMN_PROPERTY_TYPE,
    COLUMN_PROPERTY_SELECT_OPTIONS,
    COLUMN_PROPERTY_SORT,
};

class ScheduleEdit
{
    protected:
        Schedule* m_schedule;
        bool m_isReverted = false;
        SCHEDULE_EDIT_TYPE m_type;
    public:
        ScheduleEdit(Schedule* schedule, SCHEDULE_EDIT_TYPE type);
        virtual ~ScheduleEdit();
        virtual void revert();
        virtual void apply();
        bool getIsReverted() const
        {
            return m_isReverted;
        }

        SCHEDULE_EDIT_TYPE getType() const
        {
            return m_type;
        }
};

class ElementEditBase : public ScheduleEdit
{
    protected:
        SCHEDULE_TYPE m_elementType;
        size_t m_column, m_row;
    public:
        ElementEditBase(Schedule* schedule, size_t column, size_t row, SCHEDULE_TYPE elementType);

        std::pair<size_t, size_t> getPosition() const
        {
            return std::pair<size_t, size_t>(m_row, m_column);
        }

        SCHEDULE_TYPE getElementType() const
        {
            return m_elementType;
        }
};

template <typename T>
class ElementEdit : public ElementEditBase
{
    private:
        T m_previousValue;
        T m_newValue;
    public:
        ElementEdit<T>(Schedule* schedule, size_t column, size_t row, SCHEDULE_TYPE elementType, const T& previousValue, const T& newValue) : ElementEditBase(schedule, column, row, elementType) 
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
        std::vector<ElementBase*> m_elementData = {};
    public:
        RowEdit(Schedule* schedule, bool isRemove, size_t row, const std::vector<ElementBase*>& elementDataCopy);

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
        Column* m_columnData;
    public:
        ColumnEdit(Schedule* schedule, bool isRemove, size_t column, const Column& columnData);

        ~ColumnEdit() override;

        void revert() override;

        void apply() override;

        bool getIsRemove() const
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

class ColumnPropertyEdit : public ScheduleEdit
{
    private:
        size_t m_column;
        Column* m_columnData;
        Column* m_previousColumnData;
        COLUMN_PROPERTY m_editedProperty;
    public:
        ColumnPropertyEdit(Schedule* schedule, size_t column, COLUMN_PROPERTY editedProperty, const Column& previousData, const Column& newData);

        ~ColumnPropertyEdit() override;

        void revert() override;

        void apply() override;

        size_t getColumn() const
        {
            return m_column;
        }

        COLUMN_PROPERTY getEditedProperty() const
        {
            return m_editedProperty;
        }

        // Returns the Column name from the previous data
        std::string getColumnName() const;
};