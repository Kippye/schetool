#include <element_base.h>
#include <iostream>

ElementBase::ElementBase()
{

} 

ElementBase::ElementBase(SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime)
{
    m_type = type;
    m_creationDate = creationDate;
    m_creationTime = creationTime;
    m_editDate = creationDate;
    m_editTime = creationTime;
}

SCHEDULE_TYPE ElementBase::getType() const { return m_type; }
const DateContainer& ElementBase::getCreationDate() const { return m_creationDate; }
const TimeContainer& ElementBase::getCreationTime() const { return m_creationTime; }
const DateContainer& ElementBase::getEditDate() const { return m_editDate; }
const TimeContainer& ElementBase::getEditTime() const { return m_editTime; }

std::string ElementBase::getString() const
{
    return "ElementBase";
}