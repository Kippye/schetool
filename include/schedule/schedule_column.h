#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include "filter.h"
#include "element_base.h"
#include "element.h"
#include "select_container.h"
#include "select_options.h"
#include "weekday_container.h"
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
    private:
        void setupFiltersPerType();
        std::map<SCHEDULE_TYPE, std::vector<std::shared_ptr<FilterBase>>> m_filtersPerType = {};
    public:
        std::vector<ElementBase*> rows = {};
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

        // copy assignment operator
        Column& operator=(const Column& other)
        { 
            if (this != &other)
            {
                m_filtersPerType = other.getFiltersPerType();
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
            }

            // std::cout << "Copy assigned column with " << rows.size() << " elements from " << other.name << "@" << &other << " to " << name << "@" << this << std::endl;
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

        const std::map<SCHEDULE_TYPE, std::vector<std::shared_ptr<FilterBase>>>& getFiltersPerType() const;
        std::vector<std::shared_ptr<FilterBase>>& getFilters();
        const std::vector<std::shared_ptr<FilterBase>>& getFiltersConst() const;
        size_t getFilterCount() const;

        template <typename T>
        void addFilter(const Filter<T>& filter)
        {
            if (m_filtersPerType.find(type) == m_filtersPerType.end()) { printf("Column::addFilter(filter): There is no filters vector for the Column %s's type %d\n", name.c_str(), type); return; }

            m_filtersPerType.at(type).push_back(std::make_shared<Filter<T>>(filter));
        }

        template <typename T>
        void replaceFilter(size_t index, const Filter<T>& filter)
        {
            if (m_filtersPerType.find(type) == m_filtersPerType.end()) { printf("Column::replaceFilter(%zu, filter): There is no filters vector for the Column %s's type %d\n", index, name.c_str(), type); return; }
            if (index >= m_filtersPerType.at(type).size())  { printf("Column::replaceFilter(%zu): Filter index out of range\n", index); return; }

            *std::dynamic_pointer_cast<Filter<T>>(m_filtersPerType.at(type).at(index)) = filter;
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
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<SelectContainer>*)left)->getValue() > ((const Element<SelectContainer>*)right)->getValue() : ((const Element<SelectContainer>*)left)->getValue() < ((const Element<SelectContainer>*)right)->getValue();
            }
            case(SCH_WEEKDAY):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<WeekdayContainer>*)left)->getValue() > ((const Element<WeekdayContainer>*)right)->getValue() : ((const Element<WeekdayContainer>*)left)->getValue() < ((const Element<WeekdayContainer>*)right)->getValue();
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