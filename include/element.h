#pragma once

#include <element_base.h>
#include <string>
#include <type_traits>

#include <iostream>

template <typename T>
class Element : public ElementBase
{ 
    private:
        T m_value;
    public:
        Element(){}
        Element(SCHEDULE_TYPE type, T value, const DateContainer& creationDate, const TimeContainer& creationTime) : ElementBase(type, creationDate, creationTime)
        {
            m_value = value;
        }

        std::string getString() const override
        {
            if constexpr(std::is_same_v<T, TimeContainer> || std::is_same_v<T, DateContainer>)
            {
                return m_value.getString();
            }
            else if constexpr(std::is_same_v<T, SelectContainer>)
            {
                return "";
            }
            else if constexpr(std::is_same_v<T, bool>)
            {
                return m_value == true ? "True" : "False";
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
        // Returns a mutable reference to the Element's value. Only needs to be used for Select types. Otherwise, avoid.
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
            // std::cout << getString() << std::endl;
            // if constexpr(std::is_same_v<T, TimeContainer> || std::is_same_v<T, DateContainer>)
            // {
            //     std::cout << value.getString() << std::endl;
            // }
            // else if constexpr(std::is_same_v<T, SelectContainer>)
            // {
            //     std::cout << "Select" << std::endl;
            // }
            // else if constexpr(std::is_same_v<T, bool>)
            // {
            //     std::cout << (m_value == true? "True" : "False") << std::endl;
            // }
            // else if constexpr(std::is_same_v<T, std::string>)
            // {
            //     std::cout << value << std::endl;
            // }
            // else
            // {
            //     std::cout << std::to_string(value) << std::endl;
            // }            
            // std::cout << "Setting value" << std::endl;
            m_value = value;
            // std::cout << "Set value" << std::endl;
        }
};