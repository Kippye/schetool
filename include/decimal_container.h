#pragma once
#include <element.h>

class Decimal : public Element
{
    private:
        double m_value;
    public:
        Decimal()
        {

        }
        Decimal(double value)
        {
            m_value = value;
        }
        Decimal(double value, SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime) : Element(type, creationDate, creationTime) 
        {
            setValue(value);
        }

        friend bool operator<(const Decimal& left, const Decimal& right)
        {
            return left.m_value < right.m_value;
        }

        friend bool operator>(const Decimal& left, const Decimal& right)
        {
            return left.m_value > right.m_value;
        }

        double getValue() const { return m_value; };
        void setValue(double to) { m_value = to; }
};