#include <schedule_edit.h>

void ScheduleEdit::revert()
{
    m_isReverted = true;
}

void ScheduleEdit::apply()
{
    m_isReverted = false;
}

bool ScheduleEdit::getIsReverted() const
{
    return m_isReverted;
}

SCHEDULE_TYPE ElementEditBase::getElementType() const
{
    return m_elementType;
}

// RowEdit
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