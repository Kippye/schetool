#pragma once

#include "util_types.h"
#include "filter_rule_base.h"
#include "element.h"

template <typename T>
class FilterRule : public FilterRuleBase {
    private:
        T m_passValue;

    public:
        FilterRule() = delete;
        FilterRule(const T& passValue) {
            m_passValue = passValue;
        }

        bool checkPasses(const ElementBase* element,
                         const TimeWrapper& currentTime = TimeWrapper::getCurrentTime(),
                         bool useDefaultValue = false) const override {
            T value = useDefaultValue == false ? ((const Element<T>*)element)->getValue() : Element<T>::getDefaultValue();
            if (isComparisonValidForElement(element) == false) {
                return false;
            }
            // TODO: Check if the provided ElementBase is of the correct type.
            switch (m_comparison) {
                case Comparison::Is: {
                    if constexpr (has_operator_equal<T>::value)
                        return (value == m_passValue);
                    else
                        return false;
                }
                case Comparison::IsNot: {
                    if constexpr (has_operator_equal<T>::value)
                        return (value != m_passValue);
                    else
                        return false;
                }
                case Comparison::IsLessThan: {
                    if constexpr (has_operator_less<T>::value)
                        return (value < m_passValue);
                    else
                        return false;
                }
                case Comparison::IsMoreThan: {
                    if constexpr (has_operator_less<T>::value)
                        return (value > m_passValue);
                    else
                        return false;
                }
                default:
                    isComparisonValidForElement(element);
                    return false;
            }
        }
        inline std::string getString() const override {
            Element<T> valueElement = Element<T>();
            valueElement.setValue(m_passValue);
            std::string filterString = std::string(filter_consts::comparisonStrings.at(m_comparison));
            if (m_comparison != Comparison::IsEmpty && m_comparison != Comparison::ContainsToday) {
                filterString.append(" ").append(valueElement.getString());
            }
            return filterString;
        }
        T getPassValue() const {
            return m_passValue;
        }
        void setPassValue(const T& passValue) {
            m_passValue = passValue;
        }
        bool getDateCompareCurrent() const override {
            return false;
        }
        void setDatePassCompareCurrent(bool shouldCompareToCurrent) override {
            m_dateCompareCurrent = false;
        }
};

template <>
inline DateContainer FilterRule<DateContainer>::getPassValue() const {
    switch (m_comparison) {
        case Comparison::IsEmpty: {
            return DateContainer(TimeWrapper());
        }
        default:
            return m_passValue;
    }
}

template <>
inline bool FilterRule<DateContainer>::getDateCompareCurrent() const {
    return m_dateCompareCurrent;
}

template <>
inline void FilterRule<DateContainer>::setDatePassCompareCurrent(bool shouldCompareToCurrent) {
    m_dateCompareCurrent = shouldCompareToCurrent;
}

template <>
inline std::string FilterRule<DateContainer>::getString() const {
    // empty has no value at the end
    if (m_comparison == Comparison::IsEmpty) {
        return std::string(filter_consts::comparisonStrings.at(m_comparison));
    }
    // comparison string + (date / Today)
    else
    {
        return std::format("{} {}",
                           filter_consts::comparisonStrings.at(m_comparison),
                           m_dateCompareCurrent ? "Today" : getPassValue().getString(false));
    }
}

template <>
inline bool FilterRule<SingleSelectContainer>::checkPasses(const ElementBase* element,
                                                           const TimeWrapper& currentTime,
                                                           bool useDefaultValue) const {
    SingleSelectContainer value = useDefaultValue == false ? ((const Element<SingleSelectContainer>*)element)->getValue()
                                                           : Element<SingleSelectContainer>::getDefaultValue();

    switch (m_comparison) {
        case Comparison::Is: {
            return (value == m_passValue);
        }
        case Comparison::IsNot: {
            return (value != m_passValue);
        }
        case Comparison::IsEmpty: {
            return (value.getSelection().has_value() == false);
        }
        default:
            isComparisonValidForElement(element);
            return false;
    }
}

template <>
inline bool FilterRule<SelectContainer>::checkPasses(const ElementBase* element,
                                                     const TimeWrapper& currentTime,
                                                     bool useDefaultValue) const {
    SelectContainer value = useDefaultValue == false ? ((const Element<SelectContainer>*)element)->getValue()
                                                     : Element<SelectContainer>::getDefaultValue();

    switch (m_comparison) {
        case Comparison::Is: {
            return (value == m_passValue);
        }
        case Comparison::IsNot: {
            return (value != m_passValue);
        }
        case Comparison::IsEmpty: {
            return (value.getSelection().empty());
        }
        case Comparison::Contains: {
            return (value.contains(m_passValue));
        }
        case Comparison::DoesNotContain: {
            return (!value.contains(m_passValue));
        }
        default:
            isComparisonValidForElement(element);
            return false;
    }
}

template <>
inline bool FilterRule<WeekdayContainer>::checkPasses(const ElementBase* element,
                                                      const TimeWrapper& currentTime,
                                                      bool useDefaultValue) const {
    WeekdayContainer value = useDefaultValue == false ? ((const Element<WeekdayContainer>*)element)->getValue()
                                                      : Element<WeekdayContainer>::getDefaultValue();

    switch (m_comparison) {
        case Comparison::Is: {
            return (value == m_passValue);
        }
        case Comparison::IsNot: {
            return (value != m_passValue);
        }
        case Comparison::IsEmpty: {
            return (value.getSelection().empty());
        }
        case Comparison::Contains: {
            return (value.contains(m_passValue));
        }
        case Comparison::DoesNotContain: {
            return (!value.contains(m_passValue));
        }
        case Comparison::ContainsToday: {
            return (value.contains(WeekdayContainer::getCurrentSystemWeekday(currentTime)));
        }
        default:
            isComparisonValidForElement(element);
            return false;
    }
}

template <>
inline bool FilterRule<DateContainer>::checkPasses(const ElementBase* element,
                                                   const TimeWrapper& currentTime,
                                                   bool useDefaultValue) const {
    DateContainer value = useDefaultValue == false ? ((const Element<DateContainer>*)element)->getValue()
                                                   : Element<DateContainer>::getDefaultValue();

    switch (m_comparison) {
        case Comparison::Is: {
            if (m_dateCompareCurrent)
                // This is kind of weird, i know. Let me explain!
                // The checked value is a DATE. It should have no time component and current time should not affect it. So we get the UTC date of the DateContainer's "timeless" TimeWrapper.
                // The current time compared against is also a date, but it uses the TimeWrapper's time component and is local.
                // This way, when the local time is 23:59, its date will be A and when midnight comes, A + 1. The compared date stays the same.
                return value.getTimeConst().getDateUTC() ==
                    (currentTime.getIsEmpty() ? TimeWrapper::getCurrentTime() : currentTime)
                        .getLocalDate();  // Use the passed time as current unless it's empty
            else
                return (value == m_passValue);
        }
        case Comparison::IsNot: {
            if (m_dateCompareCurrent)
                return value.getTimeConst().getDateUTC() !=
                    (currentTime.getIsEmpty() ? TimeWrapper::getCurrentTime() : currentTime)
                        .getLocalDate();  // Use the passed time as current unless it's empty
            else
                return (value != m_passValue);
        }
        case Comparison::IsLessThan: {
            if (m_dateCompareCurrent)
                return value.getTimeConst().getDateUTC() <
                    (currentTime.getIsEmpty() ? TimeWrapper::getCurrentTime() : currentTime)
                        .getLocalDate();  // Use the passed time as current unless it's empty
            else
                return (value < m_passValue);
        }
        case Comparison::IsMoreThan: {
            if (m_dateCompareCurrent)
                return value.getTimeConst().getDateUTC() >
                    (currentTime.getIsEmpty() ? TimeWrapper::getCurrentTime() : currentTime)
                        .getLocalDate();  // Use the passed time as current unless it's empty
            else
                return (value > m_passValue);
        }
        case Comparison::IsEmpty: {
            return (value.getIsEmpty());
        }
        default:
            isComparisonValidForElement(element);
            return false;
    }
}