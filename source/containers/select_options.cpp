#include <select_options.h>
#include <util.h>

std::string SelectOptionsModification::getDataString() const
{
    char buffer[2048];
    if (m_optionNames.size() > 0)
    {
        sprintf(buffer, "SelectOptionsModification: {\n   Type: %d\n   First index: %zu\n   Second index: %zu\n   Option name count: %zu\n   First name: %s\n}\n", m_type, m_firstIndex, m_secondIndex, m_optionNames.size(), m_optionNames[0].c_str());
    }
    else
    {
        sprintf(buffer, "SelectOptionsModification: {\n   Type: %d\n   First index: %zu\n   Second index: %zu\n   No option names.\n}\n", m_type, m_firstIndex, m_secondIndex);
    }

    std::string s = std::string(buffer);
    s.shrink_to_fit();

    return s;
}

// Declared in select_container.h
void SelectOptionChange::replace(OPTION_MODIFICATION type, size_t firstIndex, size_t secondIndex)
{
    this->type = type;
    this->firstIndex = firstIndex;
    this->secondIndex = secondIndex;
}


SelectOptions::SelectOptions()
{

}

SelectOptions::SelectOptions(const std::vector<std::string>& options)
{
    m_options = options;
}

void SelectOptions::updateListeners()
{
    // std::cout << "Invoking callback for " << m_listeners.size() << " listeners" << std::endl;
    for (SelectContainer* listener: m_listeners)
    {
        listener->update(m_lastModification, m_options.size());
    }
}


const std::vector<std::string>& SelectOptions::getOptions() const
{
    return m_options;
}

size_t SelectOptions::getOptionCount() const
{
    return m_options.size();
}

const SelectOptionChange& SelectOptions::getLastChange() const
{
    return m_lastModification;
}

bool SelectOptions::applyModification(const SelectOptionsModification& modification)
{
    switch(modification.m_type)
    {
        case(OPTION_MODIFICATION_ADD):
        {
            if (modification.m_optionNames.size() == 0)
            {
                std::cout << "SelectOptions::applyModification: Failed to add Select option because no name was provided" << std::endl;
                return false;
            }
            return addOption(modification.m_optionNames[0]);
        }
        case(OPTION_MODIFICATION_REMOVE):
        { 
            return removeOption(modification.m_firstIndex);
        }
        case(OPTION_MODIFICATION_MOVE):
        { 
            return moveOption(modification.m_firstIndex, modification.m_secondIndex);
        }
        case(OPTION_MODIFICATION_CLEAR):
        { 
            clearOptions();
            break;
        }
        case(OPTION_MODIFICATION_REPLACE):
        { 
            replaceOptions(modification.m_optionNames);
            break;
        }
    }

    return true;
}

void SelectOptions::addListener(size_t index, SelectContainer& listener)
{
    if (index <= m_listeners.size() == false) { printf("SelectOptions::addListener: Can't add listener at index higher than size: %zu", index); return; }

    if (index == m_listeners.size())
    {
        m_listeners.push_back(&listener);
    }
    else
    {
        m_listeners.insert(m_listeners.begin() + index, &listener);
    }

    // std::cout << "Added listener at index: " << index << " count: " << m_listeners.size() << std::endl;
}

void SelectOptions::removeListener(size_t index)
{
    if (index < m_listeners.size() == false) { printf("SelectOptions::removeListener: Invalid index %zu, size is %zu", index, m_listeners.size()); return; }

    m_listeners.erase(m_listeners.begin() + index);
    // std::cout << "Removed listener at index: " << index << " count : " << m_listeners.size() << std::endl;
}

// Remove all listeners without notifying them. It's preferrable if the listeners remove themselves so only call this if there is no other option.
void SelectOptions::clearListeners()
{
    // std::cout << "Cleared callback listeners" << std::endl;
    m_listeners.clear();
}

void SelectOptions::setIsMutable(bool isMutable)
{
    m_mutable = isMutable;
}

bool SelectOptions::getIsMutable() const
{
    return m_mutable;
}

bool SelectOptions::addOption(const std::string& option)
{
    if (m_options.size() == SELECT_OPTION_COUNT_MAX) { return false; }
    
    m_options.push_back(option);
    return true;
}

bool SelectOptions::removeOption(const std::string& option)
{
    for (int i = m_options.size() - 1; i >= 0; i--)
    {
        if (m_options[i] == option)
        {
            m_lastModification.replace(OPTION_MODIFICATION_REMOVE, i, i);
            m_options.erase(m_options.begin() + i);
            break;
        }
        // failed to find the option
        else if (i == 0)
        {
            return false;
        }
    }
    updateListeners();
    return true;
}

bool SelectOptions::removeOption(size_t option)
{
    if (option < m_options.size() == false) { return false; }

    m_lastModification.replace(OPTION_MODIFICATION_REMOVE, option, option);
    m_options.erase(m_options.begin() + option);
    updateListeners();

    return true;
}

bool SelectOptions::moveOption(size_t firstIndex, size_t secondIndex)
{
    if (firstIndex < m_options.size() == false || secondIndex < m_options.size() == false) { return false; }

    m_lastModification.replace(OPTION_MODIFICATION_MOVE, firstIndex, secondIndex);
    containers::move(m_options, firstIndex, secondIndex);
    updateListeners();

    return true;
}

void SelectOptions::replaceOptions(const std::vector<std::string>& options)
{
    m_lastModification.replace(OPTION_MODIFICATION_REPLACE, 0, 0);
    m_options = options;
    updateListeners();
}

void SelectOptions::clearOptions()
{
    m_lastModification.replace(OPTION_MODIFICATION_CLEAR, 0, 0);
    m_options.clear();
    updateListeners();
}