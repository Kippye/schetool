#pragma once
#include <element.h>

class Number : public Element
{
    private:
        int m_value;
    public:
        Number()
        {

        }
        Number(int value)
        {
            m_value = value;
        }
        Number(int value, SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime) : Element(type, creationDate, creationTime) 
        {
            setValue(value);
        } 

        friend bool operator<(const Number& left, const Number& right)
        {
            return left.m_value < right.m_value;
        }

        friend bool operator>(const Number& left, const Number& right)
        {
            return left.m_value > right.m_value;
        }

        int getValue() { return m_value; }
        void setValue(int to) { m_value = to; }
};