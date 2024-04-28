#pragma once

#include <vector>
#include <string>
#include <select_container.h>

const size_t SELECT_OPTION_COUNT_MAX = 20;

class SelectOptions
{
    private:
        std::vector<std::string> m_options = {};
        SelectOptionChange m_lastModification;
        std::vector<SelectContainer*> m_listeners = {};
        bool m_mutable = false;
        void updateListeners();
    public:
        SelectOptions();
        SelectOptions(const std::vector<std::string>& options);

        const std::vector<std::string>& getOptions() const;
        size_t getOptionCount() const;
        const SelectOptionChange& getLastChange() const;
        void addListener(size_t index, SelectContainer& listener);
        void removeListener(size_t listenerID);
        void clearListeners();
        void addOption(const std::string& option);
        void removeOption(const std::string& option);
        void removeOption(size_t option);
        void moveOption(size_t firstIndex, size_t secondIndex);
        void replaceOptions(const std::vector<std::string>& options);
        void clearOptions();
        void setIsMutable(bool to);
        bool getIsMutable() const;
        // void renameOption(size_t option, const char* name);
};