#pragma once

#include <vector>
#include <string>
#include <optional>
#include "event.h"
#include "gui_constants.h"
#include "select_container.h"

struct SelectOption
{
    std::string name;
    SelectColor color;

    bool operator==(const SelectOption& other)
    {
        return name == other.name && color == other.color;
    }
};

class SelectOptionsModification
{
    private:
        friend class SelectOptions;
        friend class Column;
        OPTION_MODIFICATION m_type = OPTION_MODIFICATION_COUNT_UPDATE;
        size_t m_firstIndex = 0;
        std::optional<size_t> m_secondIndex = std::nullopt;
        std::optional<std::string> m_name = std::nullopt;
        std::optional<SelectColor> m_color = std::nullopt;
        std::optional<std::vector<SelectOption>> m_options = std::nullopt;
    
    public:
        bool operator==(const SelectOptionsModification& other)
        {
            bool others_equal = m_type == other.m_type &&
                m_firstIndex == other.m_firstIndex &&
                m_secondIndex == other.m_secondIndex &&
                m_name == other.m_name &&
                m_color == other.m_color;

            // Quit early to avoid the more expensive comparison
            if (others_equal == false)
            {
                return false;
            }

            // Both options vecs are empty therefore equal 
            if (m_options.has_value() == false && other.m_options.has_value() == false)
            {
                return true;
            }
            // Option vecs differ in emptiness or size, therefore unequal
            else if (m_options.has_value() != other.m_options.has_value() || m_options.value().size() != other.m_options.value().size())
            {
                return false;
            }
            // Compare element by element
            else
            {
                return std::equal(m_options.value().begin(), m_options.value().end(), other.m_options.value().begin());
            }
        }

        bool operator!=(const SelectOptionsModification& other)
        {
            return !(*this == other);
        }

        // Every modification requires at least a type. If no other function is applied, the first index will be 0.
        // This empty modification is good for OPTION_MODIFICATION_COUNT_UPDATE.
        SelectOptionsModification(OPTION_MODIFICATION type);

        // Set the first index (for most modifications)
        SelectOptionsModification& firstIndex(size_t index);
        // Add a second index (for OPTION_MODIFICATION_MOVE)
        SelectOptionsModification& secondIndex(size_t index);
        // Add a name (for OPTION_MODIFICATION_RENAME)
        SelectOptionsModification& name(const std::string& name);
        // Add a color (for OPTION_MODIFICATION_RECOLOR)
        SelectOptionsModification& color(SelectColor color);
        // Add SelectOption-s (REQUIRED for OPTION_MODIFICATION_ADD)
        SelectOptionsModification& options(const std::vector<SelectOption>& options);

        std::string getDataString() const;
        // Get a simple info struct about this modification, containing the type and the first and second index.
        SelectOptionUpdateInfo getUpdateInfo() const;
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
        std::optional<SelectOptionsModification> m_lastModification = std::nullopt;
        bool m_mutable = true;
    public:
        SelectOptions();
        SelectOptions(const std::vector<SelectOption>& options);

        Event<SelectOptionsModification> selectOptionModified;

        const std::vector<SelectOption>& getOptions() const;
        size_t getOptionCount() const;
        const std::optional<SelectOptionsModification>& getLastModification() const;
        // Apply a SelectOptionsModification to this SelectOptions. Returns true if the modification was applied, false otherwise.
        bool applyModification(const SelectOptionsModification& modification);
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
};