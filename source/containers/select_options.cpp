#include <format>
#include "select_options.h"
#include "util.h"

std::string SelectOptionsModification::getDataString() const
{
    std::string dataString = "SelectOptionsModification {\n";
    // Required components
    dataString.append(std::format("    Type: {}\n    First index: {}\n", (int)m_type, m_firstIndex));
    if (m_secondIndex.has_value())
    {
        dataString.append(std::format("    Second index: {}\n", m_secondIndex.value()));
    }
    if (m_name.has_value())
    {
        dataString.append(std::format("    Name: {}\n", m_name.value()));
    }
    if (m_color.has_value())
    {
        dataString.append(std::format("    Color enum: {}\n", m_color.value()));
    }
    if (m_options.has_value())
    {
        const auto& options = m_options.value();
        dataString.append(std::format("    Options (n: {}) {{\n", options.size()));
        for (const auto& option : options)
        {
            dataString.append(std::format("        Name: {}, Color enum: {}\n", option.name, option.color));
        }
        dataString.append("}\n");
    }
    dataString.append("}\n");

    return dataString;
}

SelectOptionUpdateInfo SelectOptionsModification::getUpdateInfo() const
{
    return SelectOptionUpdateInfo{m_type, m_firstIndex, m_secondIndex.value_or(0)};
}


SelectOptions::SelectOptions()
{

}

SelectOptions::SelectOptions(const std::vector<SelectOption>& options)
{
    m_options = options;
}


const std::vector<SelectOption>& SelectOptions::getOptions() const
{
    return m_options;
}

size_t SelectOptions::getOptionCount() const
{
    return m_options.size();
}

const std::optional<SelectOptionsModification>& SelectOptions::getLastModification() const
{
    return m_lastModification;
}

bool SelectOptions::applyModification(const SelectOptionsModification& modification)
{
    bool appliedSuccessfully = false;

    switch(modification.m_type)
    {
        case(OPTION_MODIFICATION_ADD):
        {
            if (modification.m_options.has_value() == false)
            {
                std::cout << "SelectOptions::applyModification: Failed to add Select option because no options were provided" << std::endl;
                appliedSuccessfully = false;
                break;
            }
            appliedSuccessfully = addOption(modification.m_options.value()[0]);
            break;
        }
        case(OPTION_MODIFICATION_REMOVE):
        { 
            appliedSuccessfully = removeOption(modification.m_firstIndex);
            break;
        }
        case(OPTION_MODIFICATION_MOVE):
        { 
            appliedSuccessfully = moveOption(modification.m_firstIndex, modification.m_secondIndex.value());
            break;
        }
        case(OPTION_MODIFICATION_RENAME):
        { 
            appliedSuccessfully = renameOption(modification.m_firstIndex, modification.m_name.value());
            break;
        }
        case(OPTION_MODIFICATION_RECOLOR):
        { 
            appliedSuccessfully = recolorOption(modification.m_firstIndex, modification.m_color.value());
            break;
        }
        case(OPTION_MODIFICATION_CLEAR):
        { 
            clearOptions();
            appliedSuccessfully = true;
            break;
        }
        case(OPTION_MODIFICATION_REPLACE):
        { 
            replaceOptions(modification.m_options.value());
            appliedSuccessfully = true;
            break;
        }
    }

    if (appliedSuccessfully)
    {
        m_lastModification = modification;
    }
    return appliedSuccessfully;
}

void SelectOptions::setIsMutable(bool isMutable)
{
    m_mutable = isMutable;
}

bool SelectOptions::getIsMutable() const
{
    return m_mutable;
}

bool SelectOptions::addOption(const SelectOption& option)
{
    // Select options can't have identical names.
    for (const auto& selectOption : m_options)
    {
        if (selectOption.name == option.name)
        {
            return false;
        }
    }
    m_options.push_back(option);
    return true;
}

bool SelectOptions::removeOption(const SelectOption& option)
{
    for (int i = m_options.size() - 1; i >= 0; i--)
    {
        if (m_options[i].name.compare(option.name) == 0)
        {
            m_options.erase(m_options.begin() + i);
            break;
        }
        // failed to find the option
        else if (i == 0)
        {
            return false;
        }
    }
    return true;
}

bool SelectOptions::removeOption(size_t option)
{
    if (option < m_options.size() == false) { return false; }

    m_options.erase(m_options.begin() + option);

    return true;
}

bool SelectOptions::moveOption(size_t firstIndex, size_t secondIndex)
{
    if (firstIndex < m_options.size() == false || secondIndex < m_options.size() == false) { return false; }

    containers::move(m_options, firstIndex, secondIndex);

    return true;
}

bool SelectOptions::renameOption(size_t index, const std::string& name)
{
    if (index < m_options.size() == false) { return false; }
    if (name.empty()) { return false; }
    // Select options can't have identical names.
    for (const auto& selectOption : m_options)
    {
        if (selectOption.name == name)
        {
            return false;
        }
    }

    m_options.at(index).name = name;

    return true;
}

bool SelectOptions::recolorOption(size_t index, SelectColor color)
{
    if (index < m_options.size() == false) { return false; }

    m_options.at(index).color = color;

    return true;
}

void SelectOptions::replaceOptions(const std::vector<SelectOption>& options)
{
    m_options = options;
}

void SelectOptions::clearOptions()
{
    m_options.clear();
}