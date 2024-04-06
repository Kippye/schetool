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
        std::cout << "added" << std::endl;
        m_schedule->setRow(m_row, m_elementData);
        std::cout << "set" << std::endl;
    }

    m_isReverted = false;
}