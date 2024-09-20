#pragma once

#include <set>
#include <cstddef>

enum OPTION_MODIFICATION
{
    OPTION_MODIFICATION_ADD,
    OPTION_MODIFICATION_REMOVE,
    OPTION_MODIFICATION_MOVE,
    OPTION_MODIFICATION_RENAME,
    OPTION_MODIFICATION_RECOLOR,
    OPTION_MODIFICATION_REPLACE,
    OPTION_MODIFICATION_CLEAR
};

struct SelectOptionUpdateInfo
{
    OPTION_MODIFICATION type = OPTION_MODIFICATION_ADD;
    size_t firstIndex = 0;
    size_t secondIndex = 0;

    public:
        void replace(OPTION_MODIFICATION type, size_t firstIndex, size_t secondIndex);
};

struct SelectContainer
{
    protected:
        std::set<size_t> m_selection = {};
        // updated from SelectOptions each update
        size_t m_optionCount = 0;
    public:
        // NOTE: Still passes if the SelectContainers are associated with different SelectOptions
        bool operator==(const SelectContainer& other)
        {
            return m_selection == other.getSelection();
        }

        bool operator!=(const SelectContainer& other)
        {
            return m_selection != other.getSelection();
        }

        friend bool operator<(const SelectContainer& left, const SelectContainer& right)
        {
            if (left.m_selection.size() == 0)
            {
                if (right.m_selection.size() > 0) { return true; }
                else { return false; }
            }
            if (right.m_selection.size() == 0)
            {
                return false;
            }

            size_t leftForemostOption = 1000;
            size_t rightForemostOption = 1000;

            for (auto& s: left.m_selection)
            {
                if (leftForemostOption == 1000 || s < leftForemostOption)
                {
                    leftForemostOption = s;
                }
            }
            for (auto& s: right.m_selection)
            {
                if (rightForemostOption == 1000 || s < rightForemostOption)
                {
                    rightForemostOption = s;
                }
            }

            return leftForemostOption < rightForemostOption;
        }

        friend bool operator>(const SelectContainer& left, const SelectContainer& right)
        {
            if (right.m_selection.size() == 0)
            {
                if (left.m_selection.size() > 0) { return true; }
                else { return false; }
            }
            if (left.m_selection.size() == 0)
            {
                return false;
            }

            size_t leftForemostOption = 1000;
            size_t rightForemostOption = 1000;

            for (auto& s: left.m_selection)
            {
                if (leftForemostOption == 1000 || s < leftForemostOption)
                {
                    leftForemostOption = s;
                }
            }
            for (auto& s: right.m_selection)
            {
                if (rightForemostOption == 1000 || s < rightForemostOption)
                {
                    rightForemostOption = s;
                }
            }

            return leftForemostOption > rightForemostOption;
        }

        const std::set<size_t> getSelection() const;
        void replaceSelection(const std::set<size_t>& selection);
        void setSelected(size_t index, bool selected);
        void update(const SelectOptionUpdateInfo& change, size_t optionCount);
        bool contains(const SelectContainer& other) const;
};
