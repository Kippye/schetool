#pragma once
#include <cstdlib>
#include <ctime>
#include <set>
#include <string>
#include <vector>

#include <iostream>

enum SELECT_MODIFICATION
{
    SELECT_MODIFICATION_REMOVE,
    SELECT_MODIFICATION_MOVE,
    SELECT_MODIFICATION_CLEAR
};

struct SelectOptionChange
{
    SELECT_MODIFICATION type;
    size_t firstIndex;
    size_t secondIndex;
    bool applied;

    public:
        void replace(SELECT_MODIFICATION type, size_t firstIndex, size_t secondIndex);
};

struct SelectOptions
{
    private:
        std::vector<std::string> m_options = {};
        SelectOptionChange m_lastModification;
        bool m_mutable;
    public:
        SelectOptions();
        SelectOptions(const std::vector<std::string>& options);

        const std::vector<std::string>& getOptions() const;
        const SelectOptionChange& getLastChange() const;
        void addOption(const std::string& option);
        void removeOption(const std::string& option);
        void removeOption(size_t option);
        void moveOption(size_t firstIndex, size_t secondIndex);
        void clearOptions();
        void setIsMutable(bool to);
        bool getIsMutable() const;
        // void renameOption(size_t option, const char* name);
        void modificationApplied();
};

class Select
{
    private:
        std::set<size_t> m_selection;
        SelectOptions* m_options;
    public:
        Select();
        Select(SelectOptions& options);
        
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

        // TEMP
        void printSelection()
        {
            for (auto s: m_selection)
            {
                std::cout << s << ": " << m_options->getOptions()[s] << std::endl;
            }
        }
        const std::set<size_t>& getSelection();
        void setSelected(size_t index, bool selected);
        void update();
};