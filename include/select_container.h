#pragma once

#include <functional>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <cstdint>

const size_t SELECT_OPTION_COUNT_MAX = 20;

enum OPTION_MODIFICATION
{
    /* NOTE: OPTION_MODIFICATION_ADD doesn't update the SelectOptionChange since Selects don't need to be updated. */
    OPTION_MODIFICATION_ADD,
    OPTION_MODIFICATION_REMOVE,
    OPTION_MODIFICATION_MOVE,
    OPTION_MODIFICATION_REPLACE,
    OPTION_MODIFICATION_CLEAR,
};

struct SelectOptionChange
{
    OPTION_MODIFICATION type;
    size_t firstIndex;
    size_t secondIndex;

    public:
        void replace(OPTION_MODIFICATION type, size_t firstIndex, size_t secondIndex);
};

struct SelectOptions
{
    private:
        std::vector<std::string> m_options = {};
        SelectOptionChange m_lastModification;
        std::map<intptr_t, std::function<void(const SelectOptionChange&)>> m_callbacks = {};
        bool m_mutable;
        void invokeCallback();
    public:
        SelectOptions();
        SelectOptions(const std::vector<std::string>& options);

        const std::vector<std::string>& getOptions() const;
        const SelectOptionChange& getLastChange() const;
        void addCallbackListener(intptr_t listenerPointer, std::function<void(const SelectOptionChange&)>& listener);
        void removeCallbackListener(intptr_t listenerPointer);
        void addOption(const std::string& option);
        void removeOption(const std::string& option);
        void removeOption(size_t option);
        void moveOption(size_t firstIndex, size_t secondIndex);
        void replaceOptions(const std::vector<std::string>& options);
        void clearOptions();
        void setIsMutable(bool to);
        bool getIsMutable() const;
        // void renameOption(size_t option, const char* name);
        void modificationApplied();
};

struct SelectContainer
{
    private:
        std::set<size_t> m_selection;
        SelectOptions* m_options;
        std::function<void(const SelectOptionChange&)> updateCallback = std::function<void(const SelectOptionChange&)>([&](const SelectOptionChange& lastChange)
        {
            update(lastChange);
        });
    public:
        SelectContainer();
        SelectContainer(SelectOptions& options);
        SelectContainer(const SelectContainer& other);
        SelectContainer& operator=(const SelectContainer& other);
        ~SelectContainer();

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

        const std::set<size_t> getSelection() const;
        void replaceSelection(const std::set<size_t>& selection);
        void setSelected(size_t index, bool selected);
        void listenToCallback();
        void update(const SelectOptionChange& change);
};