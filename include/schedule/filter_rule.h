#pragma once

#include "filter_rule_base.h"
#include "element.h"

template <typename T>
class FilterRule : public FilterRuleBase
{
    private:
        T m_passValue;
    public:
        FilterRule() = delete;
        FilterRule(const T& passValue)
        {
            m_passValue = passValue;
        }

        bool checkPasses(const ElementBase* element, const TimeWrapper& currentTime) const override
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
        inline std::string getString() const override
        {
            Element<T> valueElement = Element<T>();
            valueElement.setValue(m_passValue);
            std::string filterString = std::string(filter_consts::comparisonStrings.at(m_comparison));
            if (m_comparison != Comparison::IsEmpty && m_comparison != Comparison::ContainsToday)
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
inline std::string FilterRule<DateContainer>::getString() const
{
    std::string filterString = std::string(filter_consts::comparisonStrings.at(m_comparison));
    if (m_comparison != Comparison::IsEmpty)
    {
        filterString.append(" ").append(m_passValue.getString(m_comparison == Comparison::IsRelativeToToday));
    }
    return filterString;
}

template <>
inline bool FilterRule<SelectContainer>::checkPasses(const ElementBase* element, const TimeWrapper& currentTime) const
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
        case Comparison::IsEmpty:
        {
            return (value.getSelection().empty());
        }
        case Comparison::Contains:
        {
            return (value.contains(m_passValue));
        }
        case Comparison::DoesNotContain:
        {
            return (!value.contains(m_passValue));
        }
        default: isComparisonValidForElement(element); return false;
    }
}

template <>
inline bool FilterRule<WeekdayContainer>::checkPasses(const ElementBase* element, const TimeWrapper& currentTime) const
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
        case Comparison::IsEmpty:
        {
            return (value.getSelection().empty());
        }
        case Comparison::Contains:
        {
            return (value.contains(m_passValue));
        }
        case Comparison::DoesNotContain:
        {
            return (!value.contains(m_passValue));
        }
        case Comparison::ContainsToday:
        {
            return (value.contains(WeekdayContainer::getCurrentSystemWeekday(currentTime)));
        }
        default: isComparisonValidForElement(element); return false;
    }
}

template <>
inline bool FilterRule<DateContainer>::checkPasses(const ElementBase* element, const TimeWrapper& currentTime) const
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
            // This is kind of weird, i know. Let me explain!
            // The checked value is a DATE. It should have no time component and current time should not affect it. So we get the UTC date of the DateContainer's "timeless" TimeWrapper.
            // The current time compared against is also a date, but it uses the TimeWrapper's time component and is local. 
            // This way, when the local time is 23:59, its date will be A and when midnight comes, A + 1. The compared date stays the same.
            return (value.getTimeConst().getDateUTC() == (currentTime.getIsEmpty() ? TimeWrapper::getCurrentTime() : currentTime).getLocalDate()); // Use the passed time as current unless it's empty
        }
        default: isComparisonValidForElement(element); return false;
    }
}