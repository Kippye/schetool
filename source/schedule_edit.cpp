#include <schedule_edit.h>

ScheduleEdit::ScheduleEdit(Schedule* schedule, SCHEDULE_EDIT_TYPE type) 
{ 
    m_schedule = schedule;
    m_type = type;
}

ScheduleEdit::~ScheduleEdit()
{

}

void ScheduleEdit::revert()
{
    m_isReverted = true;
}

void ScheduleEdit::apply()
{
    m_isReverted = false;
}


// ElementEditBase
ElementEditBase::ElementEditBase(Schedule* schedule, size_t column, size_t row, SCHEDULE_TYPE elementType) : ScheduleEdit(schedule, SCHEDULE_EDIT_ELEMENT) 
{
    m_column = column;
    m_row = row;
    m_elementType = elementType;
}


// ElementEdit
template <typename T>
ElementEdit<T>::ElementEdit(Schedule* schedule, size_t column, size_t row, SCHEDULE_TYPE elementType, const T& previousValue, const T& newValue) : ElementEditBase(schedule, column, row, elementType) 
{
    m_previousValue = previousValue;
    m_newValue = newValue;
}


// RowEdit
RowEdit::RowEdit(Schedule* schedule, bool isRemove, size_t row, const std::vector<ElementBase*>& elementDataCopy) : ScheduleEdit(schedule, SCHEDULE_EDIT_ROW) 
{
    m_isRemove = isRemove;
    m_row = row;
    m_elementData = elementDataCopy;
}

RowEdit::~RowEdit()
{
    if (m_elementData.size() == 0) { return; }

    for (size_t i = m_elementData.size() - 1; i > 0; i--)
    {
        delete m_elementData[i];
    }
}

void RowEdit::revert()
{
    // reverting a removal means adding the row back
    if (m_isRemove)
    {
        m_schedule->addRow(m_row, false);
        m_schedule->setRow(m_row, m_elementData);
    }
    // reverting an addition means removing the row again
    else
    {
        m_schedule->removeRow(m_row, false);
    }

    m_isReverted = true;
} 

void RowEdit::apply()
{
    // applying a removal means removing the row
    if (m_isRemove)
    {
        m_schedule->removeRow(m_row, false);
    }
    // applying an addition means adding the row
    else
    {
        m_schedule->addRow(m_row, false);
        m_schedule->setRow(m_row, m_elementData);
    }

    m_isReverted = false;
}


// ColumnEdit
ColumnEdit::ColumnEdit(Schedule* schedule, bool isRemove, size_t column, const Column& columnData) : ScheduleEdit(schedule, SCHEDULE_EDIT_COLUMN) 
{
    m_isRemove = isRemove;
    m_column = column;
    m_columnData = new Column(columnData);
}

ColumnEdit::~ColumnEdit()
{
    delete m_columnData;
}

void ColumnEdit::revert()
{
    // reverting a removal means adding the column
    if (m_isRemove)
    {
        m_schedule->addColumn(m_column, *m_columnData, false);
    }
    // reverting an addition means removing the column
    else
    {
        m_schedule->removeColumn(m_column, false);
    }

    m_isReverted = true;
} 

void ColumnEdit::apply()
{
    // applying a removal means removing the column
    if (m_isRemove)
    {
        m_schedule->removeColumn(m_column, false);
    }
    // applying an addition means adding the column
    else
    {
        m_schedule->addColumn(m_column, *m_columnData, false);
    }

    m_isReverted = false;
}