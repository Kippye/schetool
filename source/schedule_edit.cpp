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