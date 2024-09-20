#pragma once

#include <vector>
#include <string>
#include <optional>
#include "gui_constants.h"
#include "select_container.h"

struct SelectOption
{
    std::string name;
    SelectColor color;
};

class SelectOptionsModification
{
    private:
        friend class SelectOptions;
        OPTION_MODIFICATION m_type = OPTION_MODIFICATION_ADD;
        size_t m_firstIndex = 0;
        std::optional<size_t> m_secondIndex = std::nullopt;
        std::optional<std::string> m_name = std::nullopt;
        std::optional<ImColor> m_color = std::nullopt;
        std::optional<std::vector<SelectOption>> m_options = std::nullopt;
    
    public:
        SelectOptionsModification(OPTION_MODIFICATION type);

        SelectOptionsModification& firstIndex(size_t index);
        SelectOptionsModification& secondIndex(size_t index);
        SelectOptionsModification& name(const std::string& name);
        SelectOptionsModification& color(SelectColor color);
        SelectOptionsModification& options(const std::vector<SelectOption>& options);

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

inline SelectOptionsModification& SelectOptionsModification::name(const std::string& name)
{
    m_name = name;
    return *this;
}

inline SelectOptionsModification& SelectOptionsModification::color(SelectColor color)
{
    m_color = color;
    return *this;
}

inline SelectOptionsModification& SelectOptionsModification::options(const std::vector<SelectOption>& options)
{
    m_options = options;
    return *this;
}


class SelectOptions
{
    private:
        std::vector<SelectOption> m_options = {};
        SelectOptionUpdateInfo m_lastUpdateInfo;
        std::vector<SelectContainer*> m_listeners = {};
        bool m_mutable = true;
        void updateListeners();
    public:
        SelectOptions();
        SelectOptions(const std::vector<SelectOption>& options);

        const std::vector<SelectOption>& getOptions() const;
        size_t getOptionCount() const;
        const SelectOptionUpdateInfo& getLastUpdateInfo() const;
        // Apply a SelectOptionsModification to this SelectOptions. Returns true if the modification was applied, false otherwise. NOTE:
        bool applyModification(const SelectOptionsModification& modification);
        void addListener(size_t index, SelectContainer& listener);
        void removeListener(size_t listenerID);
        void clearListeners();
        bool addOption(const SelectOption& option);
        bool removeOption(const SelectOption& option);
        bool removeOption(size_t option);
        bool moveOption(size_t firstIndex, size_t secondIndex);
        bool renameOption(size_t option, const std::string& name);
        bool recolorOption(size_t option, SelectColor color);
        void replaceOptions(const std::vector<SelectOption>& options);
        void clearOptions();
        void setIsMutable(bool to);
        bool getIsMutable() const;
        // void renameOption(size_t option, const char* name);
};