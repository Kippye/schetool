#include <element_base.h>

ElementBase::ElementBase() {
}

ElementBase::ElementBase(SCHEDULE_TYPE type, const TimeWrapper& creationTime) {
    m_type = type;
    m_creationTime = creationTime;
    m_editTime = creationTime;
}

SCHEDULE_TYPE ElementBase::getType() const {
    return m_type;
}
const TimeWrapper& ElementBase::getCreationTime() const {
    return m_creationTime;
}
const TimeWrapper& ElementBase::getEditTime() const {
    return m_editTime;
}

std::string ElementBase::getString() const {
    return "ElementBase";
}

ElementBase* ElementBase::getCopy() {
    return new ElementBase(*this);
}