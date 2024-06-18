#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include "filter.h"
#include "element_base.h"
#include "element.h"
#include "select_container.h"
#include "select_options.h"
#include "time_container.h"
#include "date_container.h"

typedef int ScheduleColumnFlags;

const size_t COLUMN_NAME_MAX_LENGTH = 64;

enum ScheduleColumnFlags_
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
    std::vector<ElementBase*> rows = {};
    std::vector<std::shared_ptr<FilterBase>> filters = {};
    SCHEDULE_TYPE type;
    std::string name;
    bool permanent = false;
    ScheduleColumnFlags flags;
    COLUMN_SORT sort;
    SelectOptions selectOptions;

    Column();
    Column(const std::vector<ElementBase*>& rows, 
        SCHEDULE_TYPE type, 
        const std::string& name,
        bool permanent = false,
        ScheduleColumnFlags flags = ScheduleColumnFlags_None,
        COLUMN_SORT sort = COLUMN_SORT_NONE,
        const SelectOptions& selectOptions = SelectOptions()
    );

    Column(const Column& other);

    Column& operator=(const Column& other)
    {
        type = other.type;
        name = other.name;
        permanent = other.permanent;
        flags = other.flags;
        sort = other.sort;
        selectOptions = other.selectOptions;

        rows.clear();

        for (size_t i = 0; i < other.rows.size(); i++)
        {
            rows.push_back(other.rows[i]->getCopy());
        }

        // std::cout << "Assigned column with " << rows.size() << " elements from " << other.name << "@" << &other << " to " << name << "@" << this << std::endl;
        return *this;
    }

    ElementBase* operator [] (size_t index)
    {
        return getElement(index);
    }

    ~Column();

    bool hasElement(size_t index) const;

    ElementBase* getElement(size_t index);

    const ElementBase* getElementConst(size_t index) const;

    template <typename T>
    void addFilter(SCHEDULE_TYPE type, const Filter<T>& filter)
    {
        filters.push_back(std::make_shared<Filter<T>>(filter));
    }

    template <typename T>
    void replaceFilter(size_t index, const Filter<T>& filter)
    {
        if (index >= filters.size())
        {
            printf("Column::replaceFilter(%zu): Filter index out of range\n", index);
            return;
        }

        *std::dynamic_pointer_cast<Filter<T>>(filters.at(index)) = filter;
    }

    void removeFilter(size_t index);
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
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<SelectContainer>*)left)->getConstValueReference() > ((const Element<SelectContainer>*)right)->getConstValueReference() : ((const Element<SelectContainer>*)left)->getConstValueReference() < ((const Element<SelectContainer>*)right)->getConstValueReference();
            }
            case(SCH_TIME):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<TimeContainer>*)left)->getValue() > ((const Element<TimeContainer>*)right)->getValue() : ((const Element<TimeContainer>*)left)->getValue() < ((const Element<TimeContainer>*)right)->getValue();
            }
            case(SCH_DATE):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<DateContainer>*)left)->getValue() > ((const Element<DateContainer>*)right)->getValue() : ((const Element<DateContainer>*)left)->getValue() < ((const Element<DateContainer>*)right)->getValue();
            }
            default:
            {
                return false;
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