#include <vector>
#include <element_base.h>

typedef int ScheduleColumnFlags;

const size_t COLUMN_NAME_MAX_LENGTH = 64;

enum ScheduleElementFlags_
{
    ScheduleColumnFlags_None     = 0,
    ScheduleColumnFlags_Name     = 1 << 0,
    ScheduleColumnFlags_Finished = 1 << 1,
    ScheduleColumnFlags_Start    = 1 << 2,
    ScheduleColumnFlags_Duration = 1 << 3,
    ScheduleColumnFlags_End      = 1 << 4,
};

typedef int COLUMN_SORT;

enum COLUMN_SORT_
{
    COLUMN_SORT_NONE,
    COLUMN_SORT_ASCENDING,
    COLUMN_SORT_DESCENDING
};

struct Column
{
    std::vector<ElementBase*> rows;
    SCHEDULE_TYPE type;
    std::string name;
    bool permanent;
    ScheduleElementFlags flags;
    COLUMN_SORT sort;
    bool sorted;
    SelectOptions selectOptions;

    Column()
    {
        type = SCH_TEXT;
        name = "Text";
    }
    Column(const std::vector<ElementBase*>& rows, 
        SCHEDULE_TYPE type, 
        const std::string& name,
        bool permanent = false,
        ScheduleElementFlags flags = ScheduleElementFlags_None,
        COLUMN_SORT sort = COLUMN_SORT_NONE,
        bool sorted = false,
        const SelectOptions& selectOptions = SelectOptions())
    {
        this->rows = rows;
        this->type = type;
        this->name = name;
        this->permanent = permanent;
        this->flags = flags;
        this->sort = sort;
        this->sorted = sorted;
        this->selectOptions = selectOptions;
    }

    Column(const Column& other)
    {
        type = other.type;
        name = other.name;
        permanent = other.permanent;
        flags = other.flags;
        sort = other.sort;
        sorted = other.sorted;
        selectOptions = other.selectOptions;

        for (size_t i = 0; i < other.rows.size(); i++)
        {
            rows.push_back(other.rows[i]->getCopy());
        }

        std::cout << "Copied column with " << rows.size() << " elements from " << other.name << "@" << &other << " to " << name << "@" << this << std::endl;
    }

    Column& operator=(const Column& other)
    {
        type = other.type;
        name = other.name;
        permanent = other.permanent;
        flags = other.flags;
        sort = other.sort;
        sorted = other.sorted;
        selectOptions = other.selectOptions;

        rows.clear();

        for (size_t i = 0; i < other.rows.size(); i++)
        {
            rows.push_back(other.rows[i]->getCopy());
        }

        std::cout << "Assigned column with " << rows.size() << " elements from " << other.name << "@" << &other << " to " << name << "@" << this << std::endl;
        return *this;
    }

    ~Column()
    {
        std::cout << "Destroying Column " << name << "@" << this << std::endl;
        for (size_t i = 0; i < rows.size(); i++)
        {
            delete rows[i];
        }
    }

    ElementBase* getElement(size_t index)
    {
        return rows[index];
    }
};

struct ColumnSortComparison 
{
    SCHEDULE_TYPE type;
    COLUMN_SORT sortDirection;

    bool operator () (const ElementBase* const left, const ElementBase* const right)
    {
        switch(type)
        {
            case(SCH_BOOL):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<bool>*)left)->getValue() > ((const Element<bool>*)right)->getValue() : ((const Element<bool>*)left)->getValue() < ((const Element<bool>*)right)->getValue();
            }
            case(SCH_NUMBER):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<int>*)left)->getValue() > ((const Element<int>*)right)->getValue() : ((const Element<int>*)left)->getValue() < ((const Element<int>*)right)->getValue();
            }
            case(SCH_DECIMAL):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<double>*)left)->getValue() > ((const Element<double>*)right)->getValue() : ((const Element<double>*)left)->getValue() < ((const Element<double>*)right)->getValue();
            }
            case(SCH_TEXT):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<std::string>*)left)->getValue() > ((const Element<std::string>*)right)->getValue() : ((const Element<std::string>*)left)->getValue() < ((const Element<std::string>*)right)->getValue();
            }
            case(SCH_SELECT):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<SelectContainer>*)left)->getValue() > ((const Element<SelectContainer>*)right)->getValue() : ((const Element<SelectContainer>*)left)->getValue() < ((const Element<SelectContainer>*)right)->getValue();
            }
            case(SCH_TIME):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<TimeContainer>*)left)->getValue() > ((const Element<TimeContainer>*)right)->getValue() : ((const Element<TimeContainer>*)left)->getValue() < ((const Element<TimeContainer>*)right)->getValue();
            }
            case(SCH_DATE):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<DateContainer>*)left)->getValue() > ((const Element<DateContainer>*)right)->getValue() : ((const Element<DateContainer>*)left)->getValue() < ((const Element<DateContainer>*)right)->getValue();
            }
        }
    }

    // Setup the sort comparison information before using it
    void setup(SCHEDULE_TYPE _type, COLUMN_SORT _sortDirection)
    {
        type = _type;
        sortDirection = _sortDirection;
    }
};