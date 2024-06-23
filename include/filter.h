#pragma once

#include "filter_base.h"
#include "element.h"

template <typename T>
class Filter : public FilterBase
{
    private:
        T m_passValue;
    public:
        Filter() = delete;
        Filter(const T& passValue)
        {
            m_passValue = passValue;
        }

        bool checkPasses(const ElementBase* element) const override
        {
            if (isComparisonValidForElement(element) == false) { return false; }
            // TODO: Check if the provided ElementBase is of the correct type.
            return (((const Element<T>*)element)->getValue() == m_passValue);
        }
        T getPassValue() const
        {
            return m_passValue; 
        }
        void setPassValue(const T& passValue)
        {
            m_passValue = passValue;
        }
};

template <>
inline bool Filter<SelectContainer>::checkPasses(const ElementBase* element) const
{
    SelectContainer value = ((const Element<SelectContainer>*)element)->getValue();

    switch(m_comparison)
    {
        case Comparison::Is:
        {
            return (value == m_passValue);
        }
        case Comparison::Contains:
        {
            return (value.contains(m_passValue));
        }
        default: isComparisonValidForElement(element); return false;
    }
}

template <>
inline bool Filter<WeekdayContainer>::checkPasses(const ElementBase* element) const
{
    WeekdayContainer value = ((const Element<WeekdayContainer>*)element)->getValue();

    switch(m_comparison)
    {
        case Comparison::Is:
        {
            return (value == m_passValue);
        }
        case Comparison::Contains:
        {
            return (value.contains(m_passValue));
        }
        case Comparison::ContainsToday:
        {
            return (value.contains(WeekdayContainer::getCurrentSystemWeekday()));
        }
        default: isComparisonValidForElement(element); return false;
    }
}

template <>
inline bool Filter<DateContainer>::checkPasses(const ElementBase* element) const
{
    DateContainer value = ((const Element<DateContainer>*)element)->getValue();

    switch(m_comparison)
    {
        case Comparison::Is:
        {
            return (value == m_passValue);
        }
        case Comparison::IsRelativeToToday:
        {
            // TODO: Handle offsets as well, maybe.
            return (value == DateContainer::getCurrentSystemDate());
        }
        default: isComparisonValidForElement(element); return false;
    }
}