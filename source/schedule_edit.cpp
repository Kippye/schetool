#include <schedule_edit.h>

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

bool ScheduleEdit::getIsReverted() const
{
    return m_isReverted;
}

SCHEDULE_EDIT_TYPE ScheduleEdit::getType() const
{
    return m_type;
}

// ElementEditBase
std::pair<size_t, size_t> ElementEditBase::getPosition() const
{
    return std::pair<size_t, size_t>(m_row, m_column);
}

SCHEDULE_TYPE ElementEditBase::getElementType() const
{
    return m_elementType;
}

// RowEdit
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