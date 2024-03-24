#include <element.h>

DateContainer::DateContainer() {}
DateContainer::DateContainer(const tm& t)
{
    time = t;
}

std::string DateContainer::getString() const
{
    char output[1024];

    std::strftime(output, sizeof(output), "%d/%m/%y", &time);

    return std::string(output);
}


TimeContainer::TimeContainer() {}
TimeContainer::TimeContainer(int h, int m)
{
    hours = h;
    minutes = m;
}
TimeContainer::TimeContainer(const tm& t)
{
    hours = t.tm_hour;
    minutes = t.tm_min; 
}

std::string TimeContainer::getString() const
{
    char output[1024];

    tm outputTime;
    outputTime.tm_hour = hours;
    outputTime.tm_min = minutes;
    std::strftime(output, sizeof(output), "%H:%M", &outputTime);

    return std::string(output);
}


Element::Element()
{

} 

Element::Element(SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime)
{
    m_type = type;
    m_creationDate = creationDate;
    m_creationTime = creationTime;
    m_editDate = creationDate;
    m_editTime = creationTime;
}

SCHEDULE_TYPE Element::getType() const { return m_type; }
const DateContainer& Element::getCreationDate() const { return m_creationDate; }
const TimeContainer& Element::getCreationTime() const { return m_creationTime; }
const DateContainer& Element::getEditDate() const { return m_editDate; }
const TimeContainer& Element::getEditTime() const { return m_editTime; }