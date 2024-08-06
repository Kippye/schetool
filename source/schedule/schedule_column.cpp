#include "schedule_column.h"

Column::Column()
{
    setupFiltersPerType();
    type = SCH_TEXT;
    name = "Text";
}

Column::Column(const std::vector<ElementBase*>& rows, 
        SCHEDULE_TYPE type, 
        const std::string& name,
        bool permanent,
        ScheduleColumnFlags flags,
        COLUMN_SORT sort,
        const SelectOptions& selectOptions,
        ColumnResetOption resetOption
)
{
    setupFiltersPerType();
    this->rows = rows;
    this->type = type;
    this->name = name;
    this->permanent = permanent;
    this->flags = flags;
    this->sort = sort;
    this->selectOptions = selectOptions;
    this->resetOption = resetOption;
}

Column::Column(const Column& other)
{
    setupFiltersPerType();
    type = other.type;
    name = other.name;
    permanent = other.permanent;
    flags = other.flags;
    sort = other.sort;
    selectOptions = other.selectOptions;
    resetOption = other.resetOption;

    for (size_t i = 0; i < other.rows.size(); i++)
    {
        rows.push_back(other.rows[i]->getCopy());
    }
    const auto& otherFiltersPerType = other.getFilterGroupsPerType();
    for (const FilterGroup& filterGroup: otherFiltersPerType.at(type))
    {
        m_filterGroupsPerType.at(type).push_back(filterGroup);
    }

    // std::cout << "Copied column with " << rows.size() << " elements from " << other.name << "@" << &other << " to " << name << "@" << this << std::endl;
}

Column::~Column()
{
    // std::cout << "Destroying Column " << name << "@" << this << std::endl;
    for (size_t i = 0; i < rows.size(); i++)
    {
        delete rows[i];
    }
}

// private function. Sets up the m_filterGroupsPerType map to contain an empty vector for each SCHEDULE_TYPE
void Column::setupFiltersPerType()
{
    for (int i = 0; i < SCH_LAST; i++)
    {
        m_filterGroupsPerType.insert_or_assign<std::vector<FilterGroup>>((SCHEDULE_TYPE)i, {});
    }
}

bool Column::hasElement(size_t index) const
{
    return index < rows.size();
}

ElementBase* Column::getElement(size_t index)
{
    if (hasElement(index) == false)
    {
        printf("The column %s has no element at index %zu", name.c_str(), index);
        return nullptr;
    }
    return rows[index];
}

const ElementBase* Column::getElementConst(size_t index) const
{
    if (hasElement(index) == false)
    {
        printf("The column %s has no element at index %zu", name.c_str(), index);
        return nullptr;
    }
    return rows[index];
}

const std::map<SCHEDULE_TYPE, std::vector<FilterGroup>>& Column::getFilterGroupsPerType() const
{
    return m_filterGroupsPerType;
}

FilterGroup& Column::getFilterGroup(size_t index)
{
    auto& filterGroups = getFilterGroups();

    if (index < filterGroups.size() == false) { throw std::out_of_range("Column::getFilterGroup(): Index out of range"); }

    return filterGroups.at(index);
}

const FilterGroup& Column::getFilterGroupConst(size_t index) const
{
    const auto& filterGroups = getFilterGroupsConst();

    if (index < filterGroups.size() == false) { throw std::out_of_range("Column::getFilterGroupConst(): Index out of range"); }

    return filterGroups.at(index);
}

std::vector<FilterGroup>& Column::getFilterGroups()
{
    if (m_filterGroupsPerType.find(type) == m_filterGroupsPerType.end()) 
    { 
        printf("Column::getFilterGroups(): Warning: There is no FilterGroup vector for the Column %s's type %d. Can't return reference early!\n", name.c_str(), type);
        throw std::out_of_range("Column::getFilterGroups():Type not present in Column::m_filterGroupsPerType");
    }

    return m_filterGroupsPerType.at(type);
}

const std::vector<FilterGroup>& Column::getFilterGroupsConst() const
{
    if (m_filterGroupsPerType.find(type) == m_filterGroupsPerType.end()) 
    { 
        printf("Column::getFiltersConst(): Warning: There is no FilterGroup vector for the Column %s's type %d. Can't return reference early!\n", name.c_str(), type); 
        throw std::out_of_range("Column::getFilterGroupsConst():Type not present in Column::m_filterGroupsPerType");
    }

    return m_filterGroupsPerType.at(type);
}

bool Column::hasFilterGroupAt(size_t index) const
{
    // correct type and index is less than FilterGroup vector size
    return (m_filterGroupsPerType.find(type) != m_filterGroupsPerType.end()) && (index < m_filterGroupsPerType.at(type).size());
}

bool Column::hasFilterAt(size_t groupIndex, size_t filterIndex) const
{
    if (hasFilterGroupAt(groupIndex) == false) { return false; }

    return getFilterGroupsConst().at(groupIndex).hasFilterAt(filterIndex);
}

size_t Column::getFilterGroupCount() const
{
    if (m_filterGroupsPerType.find(type) == m_filterGroupsPerType.end()) { printf("Column::getFilterGroupCount(): There is no FilterGroup vector for the Column %s's type %d\n", name.c_str(), type); return 0; }
   
    return m_filterGroupsPerType.at(type).size();
}

bool Column::checkElementPassesFilters(const ElementBase* element) const
{
    bool passes = true;

    for (const auto& filterGroup: m_filterGroupsPerType.at(type))
    {
        passes = passes && filterGroup.checkPasses(element);
    }

    return passes;
}

bool Column::checkElementPassesFilters(size_t index) const
{
    if (hasElement(index) == false) { return false; }

    return checkElementPassesFilters(getElementConst(index));
}

bool Column::addFilterGroup(size_t groupIndex, const FilterGroup& filterGroup)
{
    if (m_filterGroupsPerType.find(type) == m_filterGroupsPerType.end()) { printf("Column::addFilterGroup(%zu): No m_filterGroupsPerType entry for type %d\n", groupIndex, type); return false; }

    m_filterGroupsPerType.at(type).insert(m_filterGroupsPerType.at(type).begin() + groupIndex, filterGroup);
    return true;
}

bool Column::removeFilterGroup(size_t groupIndex)
{
    if (hasFilterGroupAt(groupIndex) == false) { printf("Column::removeFilterGroup(%zu): There is no FilterGroup at the given index\n", groupIndex); return false; }

    m_filterGroupsPerType.at(type).erase(m_filterGroupsPerType.at(type).begin() + groupIndex);
    return true;
}

bool Column::addFilter(size_t groupIndex, size_t filterIndex, const Filter& filter)
{
    if (hasFilterGroupAt(groupIndex) == false) { printf("Column::addFilter(%zu, %zu, filter): No FilterGroup at given index\n", groupIndex, filterIndex); return false; }

    m_filterGroupsPerType.at(type).at(groupIndex).addFilter(filterIndex, filter);
    return true;
}

bool Column::removeFilter(size_t groupIndex, size_t filterIndex)
{
    if (hasFilterGroupAt(groupIndex) == false) { printf("Column::removeFilter(%zu, %zu): There is no FilterGroup at the given index\n", groupIndex, filterIndex); return false; }
    if (hasFilterAt(groupIndex, filterIndex) == false) { printf("Column::removeFilter(%zu, %zu): Filter index out of range\n", groupIndex, filterIndex); return false; }

    m_filterGroupsPerType.at(type).at(groupIndex).removeFilter(filterIndex);
    return true;
}

bool Column::removeFilterRule(size_t groupIndex, size_t filterIndex, size_t ruleIndex)
{
    if (hasFilterGroupAt(groupIndex) == false) { printf("Column::removeFilterRule(%zu, %zu, %zu): There is no FilterGroup at the given index\n", groupIndex, filterIndex, ruleIndex); return false; }
    if (hasFilterAt(groupIndex, filterIndex) == false) { printf("Column::removeFilterRule(%zu, %zu, %zu): Filter index out of range\n", groupIndex, filterIndex, ruleIndex); return false; }
    if (ruleIndex < m_filterGroupsPerType.at(type).at(groupIndex).getFilter(filterIndex).getRules().size() == false) { printf("Column::removeFilterRule(%zu, %zu, %zu): FilterRule index out of range\n", groupIndex, filterIndex, ruleIndex); return false; }

    m_filterGroupsPerType.at(type).at(groupIndex).getFilter(filterIndex).removeRule(ruleIndex);
    return true;
}