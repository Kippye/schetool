#pragma once

#include <element_base.h>

template <typename T>
class Element : public ElementBase
{ 
    private:
        T m_value;
    public:
        Element();
        Element(SCHEDULE_TYPE type, T value, const DateContainer& creationDate, const TimeContainer& creationTime);
        std::string getString() const override;
        T getValue();
        void setValue(T);
};