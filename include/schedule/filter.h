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
            switch (m_comparison)
            {
                case Comparison::Is:
                {
                    return (((const Element<T>*)element)->getValue() == m_passValue);
                }
                case Comparison::IsNot:
                {
                    return (((const Element<T>*)element)->getValue() != m_passValue);
                }
                default: isComparisonValidForElement(element); return false;
            }
        }
        std::string getString() const override
        {
            Element<T> valueElement = Element<T>();
            valueElement.setValue(m_passValue);
            std::string filterString = std::string(filter_consts::comparisonStrings.at(m_comparison));
            if (m_comparison != Comparison::IsEmpty && m_comparison != Comparison::ContainsToday && m_comparison != Comparison::ContainsTodayOrIsEmpty)
            {
                filterString.append(" ").append(valueElement.getString());
            }
            return filterString;
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
        case Comparison::IsNot:
        {
            return (value != m_passValue);
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
        case Comparison::IsNot:
        {
            return (value != m_passValue);
        }
        case Comparison::Contains:
        {
            return (value.contains(m_passValue));
        }
        case Comparison::ContainsToday:
        {
            return (value.contains(WeekdayContainer::getCurrentSystemWeekday()));
        }
        case Comparison::ContainsTodayOrIsEmpty:
        {
            return (value.contains(WeekdayContainer::getCurrentSystemWeekday()) || value.getSelection().size() == 0);
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
        case Comparison::IsNot:
        {
            return (value != m_passValue);
        }
        case Comparison::IsEmpty:
        {
            return (value.getIsEmpty());
        }
        case Comparison::IsRelativeToToday:
        {
            // TODO: Handle offsets as well, maybe.
            return (value == DateContainer::getCurrentSystemDate());
        }
        default: isComparisonValidForElement(element); return false;
    }
}