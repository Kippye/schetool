#pragma once
#include <element.h>

class Bool : public Element
{
    private:
        bool m_value;
    public:
        Bool()
        {

        }
        Bool(bool value)
        {
            m_value = value;
        }
        Bool(bool value, SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime) : Element(type, creationDate, creationTime) 
        {
            setValue(value);
        }  

        friend bool operator<(const Bool& left, const Bool& right)
        {
            return left.m_value < right.m_value;
        }

        friend bool operator>(const Bool& left, const Bool& right)
        {
            return left.m_value > right.m_value;
        }

        bool getValue() const { return m_value; };
        void setValue(bool to) { m_value = to; }
};