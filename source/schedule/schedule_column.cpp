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
        const SelectOptions& selectOptions
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

    for (size_t i = 0; i < other.rows.size(); i++)
    {
        rows.push_back(other.rows[i]->getCopy());
    }
    const auto& otherFiltersPerType = other.getFiltersPerType();
    for (size_t i = 0; i < otherFiltersPerType.at(type).size(); i++)
    {
        auto otherFilter = otherFiltersPerType.at(type).at(i);
        switch(type)
        {
            case SCH_BOOL:
                m_filtersPerType.at(type).push_back(std::make_shared<FilterRule<bool>>(*std::dynamic_pointer_cast<FilterRule<bool>>(otherFilter)));
            break;
            case SCH_NUMBER:
                m_filtersPerType.at(type).push_back(std::make_shared<FilterRule<int>>(*std::dynamic_pointer_cast<FilterRule<int>>(otherFilter)));
            break;
            case SCH_DECIMAL:
                m_filtersPerType.at(type).push_back(std::make_shared<FilterRule<double>>(*std::dynamic_pointer_cast<FilterRule<double>>(otherFilter)));
            break;
            case SCH_TEXT:
                m_filtersPerType.at(type).push_back(std::make_shared<FilterRule<std::string>>(*std::dynamic_pointer_cast<FilterRule<std::string>>(otherFilter)));
            break;
            case SCH_SELECT:
                m_filtersPerType.at(type).push_back(std::make_shared<FilterRule<SelectContainer>>(*std::dynamic_pointer_cast<FilterRule<SelectContainer>>(otherFilter)));
            break;
            case SCH_WEEKDAY:
                m_filtersPerType.at(type).push_back(std::make_shared<FilterRule<WeekdayContainer>>(*std::dynamic_pointer_cast<FilterRule<WeekdayContainer>>(otherFilter)));
            break;
            case SCH_TIME:
                m_filtersPerType.at(type).push_back(std::make_shared<FilterRule<TimeContainer>>(*std::dynamic_pointer_cast<FilterRule<TimeContainer>>(otherFilter)));
            break;
            case SCH_DATE:
                m_filtersPerType.at(type).push_back(std::make_shared<FilterRule<DateContainer>>(*std::dynamic_pointer_cast<FilterRule<DateContainer>>(otherFilter)));
            break;
            default:
            
            break;
        }
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

// private function. Sets up the m_filtersPerType map to contain an empty vector for each SCHEDULE_TYPE
void Column::setupFiltersPerType()
{
    for (int i = 0; i < SCH_LAST; i++)
    {
        m_filtersPerType.insert_or_assign<std::vector<std::shared_ptr<FilterRuleBase>>>((SCHEDULE_TYPE)i, {});
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

const std::map<SCHEDULE_TYPE, std::vector<std::shared_ptr<FilterRuleBase>>>& Column::getFiltersPerType() const
{
    return m_filtersPerType;
}

std::vector<std::shared_ptr<FilterRuleBase>>& Column::getFilters()
{
    if (m_filtersPerType.find(type) == m_filtersPerType.end()) 
    { 
        printf("Column::getFilters(): Warning: There is no filters vector for the Column %s's type %d. Can't return reference early!\n", name.c_str(), type); 
    }

    return m_filtersPerType.at(type);
}

const std::vector<std::shared_ptr<FilterRuleBase>>& Column::getFiltersConst() const
{
    if (m_filtersPerType.find(type) == m_filtersPerType.end()) 
    { 
        printf("Column::getFiltersConst(): Warning: There is no filters vector for the Column %s's type %d. Can't return reference early!\n", name.c_str(), type); 
    }

    return m_filtersPerType.at(type);
}

size_t Column::getFilterCount() const
{
    if (m_filtersPerType.find(type) == m_filtersPerType.end()) { printf("Column::getFilterCount(): There is no filters vector for the Column %s's type %d\n", name.c_str(), type); return 0; }
   
    return m_filtersPerType.at(type).size();
}

void Column::removeFilter(size_t index)
{
    if (m_filtersPerType.find(type) == m_filtersPerType.end()) { printf("Column::removeFilter(%zu): There is no filters vector for the Column %s's type %d\n", index, name.c_str(), type); return; }
    if (index >= m_filtersPerType.at(type).size()) { printf("Column::removeFilter(%zu): FilterRule index out of range\n", index); return; }

    m_filtersPerType.at(type).erase(m_filtersPerType.at(type).begin() + index);
}