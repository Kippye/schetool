#include <element.h>
#include <string>

template <typename T>
Element<T>::Element()
{

} 

template <typename T>
Element<T>::Element(SCHEDULE_TYPE type, T value, const DateContainer& creationDate, const TimeContainer& creationTime) : ElementBase(type, creationDate, creationTime)
{
    m_value = value;
}

template <typename T>
std::string Element<T>::getString() const
{
    switch (m_type)
    {
        case (SCH_TIME):
        {
            return ((TimeContainer)m_value).getString();
        }
        case (SCH_DATE):
        {
            return ((DateContainer)m_value).getString();
        }
        case (SCH_SELECT): // Probably unneeded
        {
            return std::string("");
        }
        default: // Everything else should be easy to turn into a string
        {
            return std::to_string(m_value);
        }
    }
}

template <typename T>
T Element<T>::getValue()
{
    return m_value;
}

template <typename T>
void Element<T>::setValue(T value)
{
    m_value = value;
}