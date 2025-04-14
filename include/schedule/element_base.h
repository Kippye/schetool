#pragma once
#include "time_wrapper.h"
#include "general_constants.h"

class ElementBase {
    protected:
        SCHEDULE_TYPE m_type;
        TimeWrapper m_creationTime;
        TimeWrapper m_editTime;

    public:
        ElementBase();
        ElementBase(SCHEDULE_TYPE type, const TimeWrapper& creationTime = TimeWrapper::getCurrentTime());
        SCHEDULE_TYPE getType() const;
        const TimeWrapper& getCreationTime() const;
        const TimeWrapper& getEditTime() const;

        virtual std::string getString() const;
        virtual ElementBase* getCopy();
};