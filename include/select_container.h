#pragma once
#include <cstdlib>
#include <ctime>
#include <set>
#include <string>
#include <tuple>
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
    // TODO: i still need to loop through every row to update them if the options are updated..

    public:
        SelectOptions();
        SelectOptions(const std::vector<std::string>& options);

        const std::vector<std::string>& getOptions() const;
        const SelectOptionChange& getLastChange() const;
        void addOption(const std::string& option);
        void removeOption(const std::string& option);
        void removeOption(size_t option);
        void clear();
        // void renameOption(size_t option, const char* name);
        //void moveOption(size_t option);
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
            return true; // TODO
        }

        friend bool operator>(const Select& left, const Select& right)
        {
            return true; // TODO
        }

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