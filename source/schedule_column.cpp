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

void Column::addFilter(const std::shared_ptr<FilterBase>& filter)
{
    filters.push_back(filter);
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