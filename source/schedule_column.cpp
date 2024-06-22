#include "schedule_column.h"

Column::Column()
{
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
    for (size_t i = 0; i < other.filters.size(); i++)
    {
        switch(type)
        {
            case SCH_BOOL:
                filters.push_back(std::make_shared<Filter<bool>>(*std::dynamic_pointer_cast<Filter<bool>>(other.filters.at(i))));
            break;
            case SCH_NUMBER:
                filters.push_back(std::make_shared<Filter<int>>(*std::dynamic_pointer_cast<Filter<int>>(other.filters.at(i))));
            break;
            case SCH_DECIMAL:
                filters.push_back(std::make_shared<Filter<double>>(*std::dynamic_pointer_cast<Filter<double>>(other.filters.at(i))));
            break;
            case SCH_TEXT:
                filters.push_back(std::make_shared<Filter<std::string>>(*std::dynamic_pointer_cast<Filter<std::string>>(other.filters.at(i))));
            break;
            case SCH_SELECT:
                filters.push_back(std::make_shared<Filter<SelectContainer>>(*std::dynamic_pointer_cast<Filter<SelectContainer>>(other.filters.at(i))));
            break;
            case SCH_WEEKDAY:
                filters.push_back(std::make_shared<Filter<WeekdayContainer>>(*std::dynamic_pointer_cast<Filter<WeekdayContainer>>(other.filters.at(i))));
            break;
            case SCH_TIME:
                filters.push_back(std::make_shared<Filter<TimeContainer>>(*std::dynamic_pointer_cast<Filter<TimeContainer>>(other.filters.at(i))));
            break;
            case SCH_DATE:
                filters.push_back(std::make_shared<Filter<DateContainer>>(*std::dynamic_pointer_cast<Filter<DateContainer>>(other.filters.at(i))));
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

void Column::removeFilter(size_t index)
{
    if (index >= filters.size())
    {
        printf("Column::removeFilter(%zu): Filter index out of range\n", index);
        return;
    }

    filters.erase(filters.begin() + index);
}