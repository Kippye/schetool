#pragma once

#include <memory>
#include "filter_rule.h"

// Type to contain and manage a FilterRuleBase pointer.
class FilterRuleContainer
{
    private:
        std::shared_ptr<FilterRuleBase> m_filterRule = NULL;
        bool m_isEmpty = true;
    public:
        FilterRuleContainer()
        {

        }
        FilterRuleContainer(const FilterRuleContainer& other)
        {
            if (other.m_isEmpty)
            {
                m_isEmpty = true;
            }
            else
            {
                m_filterRule = other.m_filterRule;
                m_isEmpty = false;
            }
        }

        // copy assignment operator
        FilterRuleContainer& operator=(const FilterRuleContainer& other)
        { 
            if (this != &other)
            {
                m_filterRule = other.m_filterRule;
                m_isEmpty = other.m_isEmpty;
            }

            return *this;
        }
        bool isEmpty() const
        {
            return m_isEmpty;
        }

        // FilterRuleBase "API"

        //
        bool checkPasses(const ElementBase* element) const
        {
            if (isEmpty()) { return false; }
            return m_filterRule->checkPasses(element);
        }
        std::string getString() const
        {
            if (isEmpty()) { return ""; }
            return m_filterRule->getString();
        }
        Comparison getComparison() const
        {
            if (isEmpty()) { return Comparison::Is; }
            return m_filterRule->getComparison();
        }
        void setComparison(Comparison comparison)
        {
            if (isEmpty()) { return; }
            m_filterRule->setComparison(comparison);
        }

        template <typename T>
        FilterRuleContainer& fill(const FilterRule<T>& filterRule)
        {
            m_filterRule = std::make_shared<FilterRule<T>>(filterRule);
            m_isEmpty = false;
            return *this;
        }
        template <typename T>
        FilterRule<T> getAsType()
        {
            if (isEmpty()) 
            { 
                printf("FilterRuleContainer::getAs(): Container is empty!\n"); 
                return FilterRule<T>(T()); 
            }
            return *std::dynamic_pointer_cast<FilterRule<T>>(m_filterRule);
        }
        template <typename T>
        void setPassValue(T passValue)
        {
            if (isEmpty()) 
            { 
                printf("FilterRuleContainer::setPassValue(): Container is empty!\n"); 
                return;
            }
            std::dynamic_pointer_cast<FilterRule<T>>(m_filterRule)->setPassValue(passValue);
        }
        template <typename T>
        T getPassValue()
        {
            if (isEmpty()) 
            { 
                printf("FilterRuleContainer::setPassValue(): Container is empty!\n"); 
                return T();
            }
            return std::dynamic_pointer_cast<FilterRule<T>>(m_filterRule)->getPassValue();
        }
};