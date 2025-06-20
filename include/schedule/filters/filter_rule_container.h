#pragma once

#include <memory>
#include <stdexcept>
#include "filters/filter_rule.h"

// Type to contain and manage a FilterRuleBase pointer.
class FilterRuleContainer {
    private:
        std::shared_ptr<FilterRuleBase> m_filterRule = NULL;
        bool m_isEmpty = true;

    public:
        FilterRuleContainer() {
        }
        FilterRuleContainer(const FilterRuleContainer& other) {
            if (other.m_isEmpty) {
                m_isEmpty = true;
            } else {
                m_filterRule = other.m_filterRule;
                m_isEmpty = false;
            }
        }
        template <typename T>
        FilterRuleContainer(FilterRule<T> filterRule) {
            fill(filterRule);
        }

        // copy assignment operator
        FilterRuleContainer& operator=(const FilterRuleContainer& other) {
            if (this != &other) {
                m_filterRule = other.m_filterRule;
                m_isEmpty = other.m_isEmpty;
            }

            return *this;
        }
        bool isEmpty() const {
            return m_isEmpty;
        }

        // FilterRuleBase "API"

        //
        bool checkPasses(const ElementBase* element,
                         const TimeWrapper& currentTime = TimeWrapper::getCurrentTime(),
                         bool useDefaultValue = false) const {
            if (isEmpty()) {
                return false;
            }
            return m_filterRule->checkPasses(element, currentTime, useDefaultValue);
        }
        std::string getString() const {
            if (isEmpty()) {
                return "";
            }
            return m_filterRule->getString();
        }
        Comparison getComparison() const {
            if (isEmpty()) {
                return Comparison::Is;
            }
            return m_filterRule->getComparison();
        }
        void setComparison(Comparison comparison) {
            if (isEmpty()) {
                return;
            }
            m_filterRule->setComparison(comparison);
        }
        bool getDateCompareCurrent() const {
            if (isEmpty()) {
                return false;
            }
            return m_filterRule->getDateCompareCurrent();
        }
        void setDatePassCompareCurrent(bool shouldCompareToCurrent) {
            if (isEmpty()) {
                return;
            }
            m_filterRule->setDatePassCompareCurrent(shouldCompareToCurrent);
        }

        template <typename T>
        FilterRuleContainer& fill(const FilterRule<T>& filterRule) {
            m_filterRule = std::make_shared<FilterRule<T>>(filterRule);
            m_isEmpty = false;
            return *this;
        }
        template <typename T>
        FilterRule<T> getAsType() const {
            if (isEmpty()) {
                throw std::runtime_error("FilterRuleContainer::getAsType(): Container is empty!");
            }
            return *std::dynamic_pointer_cast<FilterRule<T>>(m_filterRule);
        }
        template <typename T>
        void setPassValue(T passValue) {
            if (isEmpty()) {
                throw std::runtime_error("FilterRuleContainer::setPassValue(): Container is empty!");
            }
            std::dynamic_pointer_cast<FilterRule<T>>(m_filterRule)->setPassValue(passValue);
        }
        template <typename T>
        T getPassValue() const {
            if (isEmpty()) {
                throw std::runtime_error("FilterRuleContainer::getPassValue(): Container is empty!");
            }
            return std::dynamic_pointer_cast<FilterRule<T>>(m_filterRule)->getPassValue();
        }
};