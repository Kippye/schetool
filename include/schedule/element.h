#pragma once

#include <string>
#include <type_traits>
#include "element_base.h"
#include "select_container.h"
#include "time_container.h"
#include "date_container.h"
#include "weekday_container.h"

template <typename T>
class Element : public ElementBase
{ 
    private:
        T m_value;
    public:
        Element(){}
        // Create an Element.
        // NOTE: Make sure that the SCHEDULE_TYPE matches the template type T!
        // If creationTime is not provided, it's assumed that the element was just created and the current time will be used.
        Element(SCHEDULE_TYPE type, const T& value, const TimeWrapper& creationTime = TimeWrapper::getCurrentTime()) : ElementBase(type, creationTime)
        {
            m_value = value;
        }

        bool operator==(const Element<T>& other) const
        {
            return m_value == other.m_value;
        }

        bool operator!=(const Element<T>& other) const
        {
            return m_value != other.m_value;
        }

        std::string getString() const override
        {
            if constexpr(std::is_same_v<T, TimeContainer> || std::is_same_v<T, DateContainer>)
            {
                return m_value.getString();
            }
            else if constexpr(std::is_same_v<T, SelectContainer> || std::is_same_v<T, WeekdayContainer>)
            {
                return "[...]";
            }
            else if constexpr(std::is_same_v<T, bool>)
            {
                return m_value == true ? "checked" : "unchecked";
            }
            else if constexpr(std::is_same_v<T, std::string>)
            {
                return m_value;
            }
            else
            {
                return std::to_string(m_value);
            }
        }

        ElementBase* getCopy() override
        {
            return new Element<T>(*this);
        }

        T getValue() const
        {
            return m_value;
        }
        // Returns a mutable reference to the Element's value.
        T& getValueReference()
        {
            return m_value;
        }
        const T& getConstValueReference() const
        {
            return m_value;
        }
        void setValue(const T& value)
        {
            m_value = value;
        }
};