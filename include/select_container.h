#pragma once
#include <element.h>
#include <cstdlib>
#include <ctime>
#include <set>
#include <string>
#include <vector>

#include <iostream>

class Select : public Element
{
    private:
        std::set<size_t> m_selection;
        SelectOptions* m_options;
    public:
        Select();
        Select(SelectOptions& options);
        Select(SelectOptions& options, SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime) : Element(type, creationDate, creationTime) 
        {
            m_options = &options;
        }
        
        friend bool operator<(const Select& left, const Select& right)
        {
            if (right.m_selection.size() == 0) { return true; }
            if (left.m_selection.size() == 0) { return false; }

            size_t leftForemostOption = 1000;
            size_t rightForemostOption = 1000;

            for (size_t i = 0; i < left.m_options->getOptions().size(); i++)
            {
                // has this option selected
                if (leftForemostOption == 1000 && left.m_selection.find(i) != left.m_selection.end())
                {
                    leftForemostOption = i;
                }
                if (rightForemostOption == 1000 && right.m_selection.find(i) != right.m_selection.end())
                {
                    rightForemostOption = i;
                }
            }

            return left.m_options->getOptions()[leftForemostOption] < right.m_options->getOptions()[rightForemostOption];
        }

        friend bool operator>(const Select& left, const Select& right)
        {
            if (right.m_selection.size() == 0) { return false; }
            if (left.m_selection.size() == 0) { return true; }

            size_t leftForemostOption = 1000;
            size_t rightForemostOption = 1000;

            for (size_t i = 0; i < left.m_options->getOptions().size(); i++)
            {
                // has this option selected
                if (leftForemostOption == 1000 && left.m_selection.find(i) != left.m_selection.end())
                {
                    leftForemostOption = i;
                }
                if (rightForemostOption == 1000 && right.m_selection.find(i) != right.m_selection.end())
                {
                    rightForemostOption = i;
                }
            }

            return left.m_options->getOptions()[leftForemostOption] > right.m_options->getOptions()[rightForemostOption];
        }

        const std::set<size_t>& getSelection() const;
        void replaceSelection(const std::set<size_t>& selection);
        void setSelected(size_t index, bool selected);
        void update();
};