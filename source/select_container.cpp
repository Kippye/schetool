#include <select_container.h>
#include <util.h>

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

void SelectOptions::invokeCallback()
{
    std::cout << "Invoking callback for " << m_callbacks.size() << " listeners" << std::endl;
    for (auto& listener: m_callbacks)
    {
        listener.second(m_lastModification);
    }
}


const std::vector<std::string>& SelectOptions::getOptions() const
{
    return m_options;
}

const SelectOptionChange& SelectOptions::getLastChange() const
{
    return m_lastModification;
}

void SelectOptions::addCallbackListener(intptr_t listenerPointer, std::function<void(const SelectOptionChange&)>& listener)
{
    if (m_callbacks.find(listenerPointer) != m_callbacks.end())
    {
        std::cout << "SelectOptions::addCallbackListener: A callback listener already exists with the given key. Replacing it is not supported behaviour" << std::endl;
        return;
    }
    m_callbacks.insert({listenerPointer, listener});
    std::cout << "Added callback listener @" << listenerPointer << " count: " << m_callbacks.size() << std::endl;
}

void SelectOptions::removeCallbackListener(intptr_t listenerPointer)
{
    if (m_callbacks.find(listenerPointer) != m_callbacks.end())
    {
        m_callbacks.erase(listenerPointer);
        std::cout << "Removed callback listener @" << listenerPointer << " count : " << m_callbacks.size() << std::endl;
    }
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
    invokeCallback();
}

void SelectOptions::removeOption(size_t option)
{
    m_lastModification.replace(OPTION_MODIFICATION_REMOVE, option, option);
    m_options.erase(m_options.begin() + option);
    invokeCallback();
}

void SelectOptions::moveOption(size_t firstIndex, size_t secondIndex)
{
    m_lastModification.replace(OPTION_MODIFICATION_MOVE, firstIndex, secondIndex);
    containers::move(m_options, firstIndex, secondIndex);
    invokeCallback();
}

void SelectOptions::replaceOptions(const std::vector<std::string>& options)
{
    m_lastModification.replace(OPTION_MODIFICATION_REPLACE, 0, 0);
    m_options = options;
    invokeCallback();
}

void SelectOptions::clearOptions()
{
    m_lastModification.replace(OPTION_MODIFICATION_CLEAR, 0, 0);
    m_options.clear();
    invokeCallback();
}


SelectContainer::SelectContainer()
{

}

SelectContainer::SelectContainer(SelectOptions& options) 
{ 
    m_options = &options;
    //m_options->addCallbackListener(intptr_t(this), updateCallback);
}

SelectContainer::SelectContainer(const SelectContainer& other) : SelectContainer(*other.m_options)
{
    m_selection = other.m_selection;
}

SelectContainer& SelectContainer::operator=(const SelectContainer& other)
{
    m_options = other.m_options;
    //m_options->addCallbackListener(intptr_t(this), updateCallback);
    m_selection = other.m_selection;

    return *this;
}

SelectContainer::~SelectContainer()
{
    m_options->removeCallbackListener(intptr_t(this));
}

const std::set<size_t> SelectContainer::getSelection() const
{
    return m_selection;
}

void SelectContainer::setSelected(size_t index, bool select)
{
    if (index > m_options->getOptions().size() - 1)
    {
        std::cout << "Tried to change selection of an option that does not exist" << std::endl;
        return;
    }

    auto indexInSelection = m_selection.find(index);
    // already in selection
    if (indexInSelection != m_selection.end())
    {
        if (select == false)
        {
            m_selection.erase(indexInSelection);
        }
    }
    // not in selection yet
    else
    {
        if (select == true)
        {
            m_selection.insert(index);
        }
    }
}

void SelectContainer::replaceSelection(const std::set<size_t>& selection)
{
    m_selection = selection;
}

void SelectContainer::listenToCallback()
{
    m_options->addCallbackListener(intptr_t(this), updateCallback);
}

// Update the SelectContainer to recorrect its indices after a modification to the attached SelectOptions
void SelectContainer::update(const SelectOptionChange& lastChange)
{
    std::cout << "UPDATE " << lastChange.type << std::endl;

    switch(lastChange.type)
    {
        case(OPTION_MODIFICATION_ADD):
        {
            // Currently, options can only be "pushed back" so no selections are invalidated. We chillin.
            break;
        }
        // An option was removed. Reduce all indices after the removed by 1
        case (OPTION_MODIFICATION_REMOVE):
        {
            if (m_selection.find(lastChange.firstIndex) != m_selection.end()) 
            { 
                m_selection.erase(lastChange.firstIndex); 
            }

            for (size_t i = lastChange.firstIndex + 1; i < m_options->getOptions().size() + 1; i++)
            {
                if (m_selection.find(i) != m_selection.end())
                {
                    m_selection.erase(i);
                    m_selection.insert(i - 1);
                }
            }
            break;
        }
        // An option was moved from one index to another. 
        case (OPTION_MODIFICATION_MOVE):
        {
            bool addSecondIndex = false;

            if (m_selection.find(lastChange.firstIndex) != m_selection.end())
            {
                m_selection.erase(lastChange.firstIndex);
                addSecondIndex = true;
            }

            // If firstIndex > secondIndex, add 1 to every index between firstIndex (excluded) and secondIndex (included).
            if (lastChange.firstIndex > lastChange.secondIndex)
            {
                for (size_t i = lastChange.secondIndex; i < lastChange.firstIndex; i++)
                {
                    if (m_selection.find(i) != m_selection.end())
                    {
                        m_selection.erase(i);
                        m_selection.insert(i + 1);
                    }
                }
            }
            // If firstIndex < secondIndex, subtract 1 from every index between firstIndex (excluded) and lastIndex (included).
            else if (lastChange.firstIndex < lastChange.secondIndex)
            {
                for (size_t i = lastChange.secondIndex; i > lastChange.firstIndex; i--)
                {
                    if (m_selection.find(i) != m_selection.end())
                    {
                        m_selection.erase(i);
                        m_selection.insert(i - 1);
                    }
                }
            }

            // add the index that the (selected) option was moved to
            if (addSecondIndex)
            {
                m_selection.insert(lastChange.secondIndex);
            }
            break;
        }
        case (OPTION_MODIFICATION_REPLACE):
        {
            // TODO HOW??
        }
        case (OPTION_MODIFICATION_CLEAR):
        {
            m_selection.clear();
            break;
        }
    }
    std::cout << "Finished update" << std::endl;
}