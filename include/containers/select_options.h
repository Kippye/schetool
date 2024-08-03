#pragma once

#include <vector>
#include <string>
#include <select_container.h>

class SelectOptionsModification
{
    private:
        friend class SelectOptions;
        OPTION_MODIFICATION m_type = OPTION_MODIFICATION_ADD;
        size_t m_firstIndex = 0;
        size_t m_secondIndex = 0;
        std::vector<std::string> m_optionNames = {};
    
    public:
        SelectOptionsModification(OPTION_MODIFICATION type);

        SelectOptionsModification& firstIndex(size_t index);
        SelectOptionsModification& secondIndex(size_t index);
        SelectOptionsModification& optionNames(const std::vector<std::string>& optionNames);

        std::string getDataString() const;
};

inline SelectOptionsModification::SelectOptionsModification(OPTION_MODIFICATION type)
{
    m_type = type;
}

inline SelectOptionsModification& SelectOptionsModification::firstIndex(size_t index)
{
    m_firstIndex = index;
    return *this;
}

inline SelectOptionsModification& SelectOptionsModification::secondIndex(size_t index)
{
    m_secondIndex = index;
    return *this;
}

inline SelectOptionsModification& SelectOptionsModification::optionNames(const std::vector<std::string>& optionNames)
{
    m_optionNames = optionNames;
    return *this;
}


class SelectOptions
{
    private:
        std::vector<std::string> m_options = {};
        SelectOptionChange m_lastModification;
        std::vector<SelectContainer*> m_listeners = {};
        bool m_mutable = true;
        void updateListeners();
    public:
        SelectOptions();
        SelectOptions(const std::vector<std::string>& options);

        const std::vector<std::string>& getOptions() const;
        size_t getOptionCount() const;
        const SelectOptionChange& getLastChange() const;
        // Apply a SelectOptionsModification to this SelectOptions. Returns true if the modification was applied, false otherwise. NOTE:
        bool applyModification(const SelectOptionsModification& modification);
        void addListener(size_t index, SelectContainer& listener);
        void removeListener(size_t listenerID);
        void clearListeners();
        bool addOption(const std::string& option);
        bool removeOption(const std::string& option);
        bool removeOption(size_t option);
        bool moveOption(size_t firstIndex, size_t secondIndex);
        void replaceOptions(const std::vector<std::string>& options);
        void clearOptions();
        void setIsMutable(bool to);
        bool getIsMutable() const;
        // void renameOption(size_t option, const char* name);
};