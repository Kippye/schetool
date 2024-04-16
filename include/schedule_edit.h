#pragma once

#include "element_base.h"
#include <element.h>
#include <schedule_column.h>

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

// TODO: somehow check for these being nullptrs
// A struct containing function pointers to functions in the Schedule. These should ALWAYS be non-null pointers.
struct ScheduleEditFunctions
{
    void (*addRow)(size_t index, bool addToHistory);
    void (*setRow)(size_t index, std::vector<ElementBase*> elementData);
    void (*removeRow)(size_t index, bool addToHistory);

    void (*addColumn)(size_t index, const Column& column, bool addToHistory);
    void (*removeColumn)(size_t column, bool addToHistory);

    void (*setColumnName)(size_t column, const char* name, bool addToHistory);
    void (*setColumnType)(size_t column, SCHEDULE_TYPE type, bool addToHistory );
    void (*modifyColumnSelectOptions)(size_t column, OPTION_MODIFICATION selectModification, size_t firstIndex, size_t secondIndex, const std::vector<std::string>& optionNames, bool addToHistory);
    void (*setColumnSort)(size_t column, COLUMN_SORT sortDirection, bool addToHistory);
};

class ScheduleEdit
{
    protected:
        bool m_isReverted = false;
        SCHEDULE_EDIT_TYPE m_type;
    public:
        ScheduleEdit(SCHEDULE_EDIT_TYPE type);
        virtual ~ScheduleEdit();
        virtual void revert(const ScheduleEditFunctions& editFunctions);
        virtual void apply(const ScheduleEditFunctions& editFunctions);
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
        ElementEdit<T>(size_t column, size_t row, SCHEDULE_TYPE elementType, const T& previousValue, const T& newValue) : ElementEditBase(schedule, column, row, elementType) 
        {
            m_previousValue = previousValue;
            m_newValue = newValue;
        }

        void revert(const ScheduleEditFunctions& editFunctions) override
        {
            // TODO: m_editFunctions.setElementValue(m_column, m_row, m_previousValue, true, false);
            m_isReverted = true;
        }

        void apply(const ScheduleEditFunctions& editFunctions) override
        {
            // TODO: m_editFunctions.setElementValue(m_column, m_row, m_newValue, true, false);
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
        RowEdit(bool isRemove, size_t row, const std::vector<ElementBase*>& elementDataCopy);

        ~RowEdit() override;

        void revert(const ScheduleEditFunctions& editFunctions) override;

        void apply(const ScheduleEditFunctions& editFunctions) override;

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
        ColumnEdit(bool isRemove, size_t column, const Column& columnData);

        ~ColumnEdit() override;

        void revert(const ScheduleEditFunctions& editFunctions) override;

        void apply(const ScheduleEditFunctions& editFunctions) override;

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
        ColumnPropertyEdit(size_t column, COLUMN_PROPERTY editedProperty, const Column& previousData, const Column& newData);

        ~ColumnPropertyEdit() override;

        void revert(const ScheduleEditFunctions& editFunctions) override;

        void apply(const ScheduleEditFunctions& editFunctions) override;

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