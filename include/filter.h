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

        bool checkPasses(const ElementBase* element) override
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
class Filter<DateContainer> : public FilterBase
{
    private:
        DateContainer m_passValue;
    public:
        Filter<DateContainer>() = delete;
        Filter<DateContainer>(const DateContainer& passValue)
        {
            m_passValue = passValue;
        }

        bool checkPasses(const ElementBase* element) override
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
            }
            // TODO: Check if the provided ElementBase is of the correct type.
        }
        DateContainer getPassValue() const
        {
            return m_passValue; 
        }
        void setPassValue(const DateContainer& passValue)
        {
            m_passValue = passValue;
        }
};