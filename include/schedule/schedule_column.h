#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <optional>
#include "schedule_constants.h"
#include "filters/filter_group.h"
#include "element_base.h"
#include "element.h"
#include "select_container.h"
#include "select_options.h"
#include "weekday_container.h"
#include "time_container.h"
#include "date_container.h"

typedef int ScheduleColumnFlags;

const size_t COLUMN_NAME_MAX_LENGTH = 64;

enum ScheduleColumnFlags_ {
    ScheduleColumnFlags_None = 0,
    ScheduleColumnFlags_Name = 1 << 0,
    ScheduleColumnFlags_Finished = 1 << 1,
    ScheduleColumnFlags_Start = 1 << 2,
    ScheduleColumnFlags_Duration = 1 << 3,
    ScheduleColumnFlags_End = 1 << 4,
};

typedef int COLUMN_SORT;

enum COLUMN_SORT_ {
    COLUMN_SORT_NONE,
    COLUMN_SORT_ASCENDING,
    COLUMN_SORT_DESCENDING
};

struct Column {
    private:
        void setupFiltersPerType();
        std::map<SCHEDULE_TYPE, std::vector<FilterGroup>> m_filterGroupsPerType = {};

    public:
        std::vector<ElementBase*> rows = {};
        SCHEDULE_TYPE type;
        std::string name;
        bool permanent = false;
        ScheduleColumnFlags flags;
        COLUMN_SORT sort;
        SelectOptions selectOptions;
        ColumnResetOption resetOption = ColumnResetOption::Never;

        Column();
        Column(const std::vector<ElementBase*>& rows,
               SCHEDULE_TYPE type,
               const std::string& name,
               bool permanent = false,
               ScheduleColumnFlags flags = ScheduleColumnFlags_None,
               COLUMN_SORT sort = COLUMN_SORT_NONE,
               const SelectOptions& selectOptions = SelectOptions(),
               ColumnResetOption resetSetting = ColumnResetOption::Never);

        Column(const Column& other);

        // copy assignment operator
        Column& operator=(const Column& other) {
            if (this != &other) {
                m_filterGroupsPerType = other.getFilterGroupsPerType();
                type = other.type;
                name = other.name;
                permanent = other.permanent;
                flags = other.flags;
                sort = other.sort;
                selectOptions = other.selectOptions;
                resetOption = other.resetOption;

                for (size_t i = 0; i < rows.size(); i++) {
                    delete rows[i];
                }

                rows.clear();

                for (size_t i = 0; i < other.rows.size(); i++) {
                    rows.push_back(other.rows[i]->getCopy());
                }
            }

            // std::cout << "Copy assigned column with " << rows.size() << " elements from " << other.name << "@" << &other << " to " << name << "@" << this << std::endl;
            return *this;
        }

        ~Column();

        // ELEMENTS
        ElementBase* operator[](size_t index) {
            return getElement(index);
        }

        bool hasElement(size_t index) const;

        // Add an element to the column. The column will handle any special cases and initialising the elements.
        // Returns true if the element was added.
        bool addElement(size_t index, ElementBase* element);

        ElementBase* getElement(size_t index);

        const ElementBase* getElementConst(size_t index) const;

        // SELECT OPTIONS
        // Applies a modification to this column's SelectOptions and updates its select elements if the modification is applied successfully
        bool modifySelectOptions(const SelectOptionsModification& modification);

        // FILTERS
        const std::map<SCHEDULE_TYPE, std::vector<FilterGroup>>& getFilterGroupsPerType() const;
        // Gets the FilterGroup at the given index.
        FilterGroup& getFilterGroup(size_t index);
        // Gets the FilterGroup at the given index.
        const FilterGroup& getFilterGroupConst(size_t index) const;
        std::vector<FilterGroup>& getFilterGroups();
        const std::vector<FilterGroup>& getFilterGroupsConst() const;

        size_t getFilterGroupCount() const;
        size_t getFilterCount() const;
        size_t getFilterRuleCount() const;

        // Uses the index to get the element itself.
        // Returns false if no element exists at the index or the element doesn't pass at least one filter.
        // Returns true if the element passes all FilterGroups.
        // Optionally pass a TimeWrapper to use instead of TimeWrapper::getCurrentTime() in filters.
        bool checkElementPassesFilters(size_t elementIndex, const std::optional<TimeWrapper>& currentTime = std::nullopt) const;

        bool hasFilterGroupAt(size_t index) const;
        bool hasFilterAt(size_t groupIndex, size_t filterIndex) const;

        bool addFilterGroup(size_t groupIndex, const FilterGroup& filterGroup);
        bool removeFilterGroup(size_t index);

        bool addFilter(size_t groupIndex, size_t filterIndex, const Filter& filter);
        bool removeFilter(size_t groupIndex, size_t filterIndex);

        template <typename T>
        bool addFilterRule(size_t groupIndex, size_t filterIndex, size_t ruleIndex, const FilterRule<T>& filterRule) {
            if (hasFilterGroupAt(groupIndex) == false) {
                printf("Column::addFilterRule(%zu, %zu, filterRule): There is no FilterGroup at the given index\n",
                       groupIndex,
                       filterIndex);
                return false;
            }
            if (hasFilterAt(groupIndex, filterIndex) == false) {
                printf("Column::addFilterRule(%zu, %zu, filterRule): There is no Filter at the given indices\n",
                       groupIndex,
                       filterIndex);
                return false;
            }

            m_filterGroupsPerType.at(type).at(groupIndex).getFilter(filterIndex).addRule(ruleIndex, filterRule);
            return true;
        }

        template <typename T>
        bool replaceFilterRule(size_t groupIndex, size_t filterIndex, size_t ruleIndex, const FilterRule<T>& filter) {
            if (hasFilterGroupAt(groupIndex) == false) {
                printf("Column::replaceFilterRule(%zu, %zu, filterRule): There is no FilterGroup at the given index\n",
                       groupIndex,
                       filterIndex);
                return false;
            }
            if (hasFilterAt(groupIndex, filterIndex) == false) {
                printf("Column::replaceFilterRule(%zu, %zu, filterRule): There is no Filter at the given indices\n",
                       groupIndex,
                       filterIndex);
                return false;
            }

            m_filterGroupsPerType.at(type).at(groupIndex).getFilter(filterIndex).replaceRule(ruleIndex, filter);
            return true;
        }

        bool removeFilterRule(size_t groupIndex, size_t filterIndex, size_t ruleIndex);
};

struct ColumnSortComparison {
        SCHEDULE_TYPE type;
        COLUMN_SORT sortDirection;

        bool operator()(const ElementBase* const left, const ElementBase* const right) {
            switch (type) {
                case (SCH_BOOL): {
                    return sortDirection == COLUMN_SORT_DESCENDING
                        ? ((const Element<bool>*)left)->getValue() > ((const Element<bool>*)right)->getValue()
                        : ((const Element<bool>*)left)->getValue() < ((const Element<bool>*)right)->getValue();
                }
                case (SCH_NUMBER): {
                    return sortDirection == COLUMN_SORT_DESCENDING
                        ? ((const Element<int>*)left)->getValue() > ((const Element<int>*)right)->getValue()
                        : ((const Element<int>*)left)->getValue() < ((const Element<int>*)right)->getValue();
                }
                case (SCH_DECIMAL): {
                    return sortDirection == COLUMN_SORT_DESCENDING
                        ? ((const Element<double>*)left)->getValue() > ((const Element<double>*)right)->getValue()
                        : ((const Element<double>*)left)->getValue() < ((const Element<double>*)right)->getValue();
                }
                case (SCH_TEXT): {
                    return sortDirection == COLUMN_SORT_DESCENDING
                        ? ((const Element<std::string>*)left)->getValue() > ((const Element<std::string>*)right)->getValue()
                        : ((const Element<std::string>*)left)->getValue() < ((const Element<std::string>*)right)->getValue();
                }
                case (SCH_SELECT): {
                    return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<SingleSelectContainer>*)left)->getValue() >
                            ((const Element<SingleSelectContainer>*)right)->getValue()
                                                                   : ((const Element<SingleSelectContainer>*)left)->getValue() <
                            ((const Element<SingleSelectContainer>*)right)->getValue();
                }
                case (SCH_MULTISELECT): {
                    return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<SelectContainer>*)left)->getValue() >
                            ((const Element<SelectContainer>*)right)->getValue()
                                                                   : ((const Element<SelectContainer>*)left)->getValue() <
                            ((const Element<SelectContainer>*)right)->getValue();
                }
                case (SCH_WEEKDAY): {
                    return sortDirection == COLUMN_SORT_DESCENDING ? ((const Element<WeekdayContainer>*)left)->getValue() >
                            ((const Element<WeekdayContainer>*)right)->getValue()
                                                                   : ((const Element<WeekdayContainer>*)left)->getValue() <
                            ((const Element<WeekdayContainer>*)right)->getValue();
                }
                case (SCH_TIME): {
                    return sortDirection == COLUMN_SORT_DESCENDING
                        ? ((const Element<TimeContainer>*)left)->getValue() > ((const Element<TimeContainer>*)right)->getValue()
                        : ((const Element<TimeContainer>*)left)->getValue() <
                            ((const Element<TimeContainer>*)right)->getValue();
                }
                case (SCH_DATE): {
                    return sortDirection == COLUMN_SORT_DESCENDING
                        ? ((const Element<DateContainer>*)left)->getValue() > ((const Element<DateContainer>*)right)->getValue()
                        : ((const Element<DateContainer>*)left)->getValue() <
                            ((const Element<DateContainer>*)right)->getValue();
                }
                default: {
                    return false;
                }
            }
        }

        // Setup the sort comparison information before using it
        void setup(SCHEDULE_TYPE _type, COLUMN_SORT _sortDirection) {
            type = _type;
            sortDirection = _sortDirection;
        }
};