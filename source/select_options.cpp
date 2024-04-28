#include <select_options.h>
#include <util.h>

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

void SelectOptions::addOption(const std::string& option)
{
    if (m_options.size() == SELECT_OPTION_COUNT_MAX) { return; }
    
    m_options.push_back(option);
}

void SelectOptions::removeOption(const std::string& option)
{
    for (int i = m_options.size() - 1; i >= 0; i--)
    {
        if (m_options[i] == option)
        {
            m_lastModification.replace(OPTION_MODIFICATION_REMOVE, i, i);
            m_options.erase(m_options.begin() + i);
            break;
        }
    }
    updateListeners();
}

void SelectOptions::removeOption(size_t option)
{
    m_lastModification.replace(OPTION_MODIFICATION_REMOVE, option, option);
    m_options.erase(m_options.begin() + option);
    updateListeners();
}

void SelectOptions::moveOption(size_t firstIndex, size_t secondIndex)
{
    m_lastModification.replace(OPTION_MODIFICATION_MOVE, firstIndex, secondIndex);
    containers::move(m_options, firstIndex, secondIndex);
    updateListeners();
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