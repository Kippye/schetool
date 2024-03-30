#pragma once
#include <string>
#include <element.h>

class Text : public Element
{
    private:
        std::string m_value;
    public:
        Text()
        {

        }
        Text(const std::string& value)
        {
            m_value = value;
        }
        Text(const std::string& value, SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime) : Element(type, creationDate, creationTime) 
        {
            m_value = value;
        }

        friend bool operator<(const Text& left, const Text& right)
        {
            return left.m_value < right.m_value;
        }

        friend bool operator>(const Text& left, const Text& right)
        {
            return left.m_value > right.m_value;
        }

        const std::string& getValue() const { return m_value; }
        void setValue(const std::string& to) { m_value = to; }
};