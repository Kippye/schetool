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
            // TODO: Check if the provided ElementBase is of the correct type.
            return (((const Element<DateContainer>*)element)->getValue() == m_passValue);
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