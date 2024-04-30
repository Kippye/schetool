#pragma once

#include <element_base.h>
#include <element.h>
#include <schedule_column.h>
#include <schedule_core.h>

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
        bool m_isReverted = false;
        SCHEDULE_EDIT_TYPE m_type;
    public:
        ScheduleEdit(SCHEDULE_EDIT_TYPE type);
        virtual ~ScheduleEdit();
        virtual void revert(ScheduleCore* const scheduleCore);
        virtual void apply(ScheduleCore* const scheduleCore);
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
        ElementEditBase(size_t column, size_t row, SCHEDULE_TYPE elementType);

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
        ElementEdit<T>(size_t column, size_t row, SCHEDULE_TYPE elementType, const T& previousValue, const T& newValue) : ElementEditBase(column, row, elementType) 
        {
            m_previousValue = previousValue;
            m_newValue = newValue;
        }

        void revert(ScheduleCore* const scheduleCore) override
        {
            scheduleCore->setElementValue(m_column, m_row, m_previousValue, true);
            m_isReverted = true;
        }

        void apply(ScheduleCore* const scheduleCore) override
        {
            scheduleCore->setElementValue(m_column, m_row, m_newValue, true);
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
        RowEdit(bool isRemove, size_t row, const std::vector<ElementBase*>& elementDataToCopy);

        ~RowEdit() override;

        void revert(ScheduleCore* const scheduleCore) override;

        void apply(ScheduleCore* const scheduleCore) override;

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
        Column m_columnData;
    public:
        ColumnEdit(bool isRemove, size_t column, const Column& columnData);

        void revert(ScheduleCore* const scheduleCore) override;

        void apply(ScheduleCore* const scheduleCore) override;

        bool getIsRemove() const
        {
            return m_isRemove;
        }

        size_t getColumn() const
        {
            return m_column;
        }

        const Column& getColumnData() const
        {
            return m_columnData;
        }
};

class ColumnPropertyEdit : public ScheduleEdit
{
    private:
        size_t m_column;
        Column m_columnData;
        Column m_previousColumnData;
        COLUMN_PROPERTY m_editedProperty;
    public:
        ColumnPropertyEdit(size_t column, COLUMN_PROPERTY editedProperty, const Column& previousData, const Column& newData);

        void revert(ScheduleCore* const scheduleCore) override;

        void apply(ScheduleCore* const scheduleCore) override;

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