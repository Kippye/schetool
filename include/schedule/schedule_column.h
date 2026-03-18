#pragma once

#include <vector>
#include <format>
#include <memory>
#include <optional>
#include <iterator>
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
    ScheduleColumnFlags_Date = 1 << 4,
    ScheduleColumnFlags_End = 1 << 5,
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
        std::vector<std::shared_ptr<ElementBase>> m_rows = {};

    public:
        SCHEDULE_TYPE type;
        std::string name;
        bool permanent = false;
        ScheduleColumnFlags flags;
        COLUMN_SORT sort;
        SelectOptions selectOptions;
        ColumnResetOption resetOption = ColumnResetOption::Never;

        Column();
        Column(const std::vector<std::shared_ptr<ElementBase>>& rows,
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

                m_rows.clear();

                for (size_t i = 0; i < other.m_rows.size(); i++) {
                    m_rows.push_back(other.m_rows[i]->getCopy());
                }
            }

            // std::cout << "Copy assigned column with " << rows.size() << " elements from " << other.name << "@" << &other << " to " << name << "@" << this << std::endl;
            return *this;
        }

        // ELEMENTS
        size_t getRowCount() const;

        bool hasElement(size_t index) const;

        template <typename T>
        bool addElement(Element<T> element) {
            return addElement(m_rows.size(), element);
        }

        template <typename T>
        bool addElement(size_t index, Element<T> element) {
            if (index <= m_rows.size() == false) {
                return false;
            }
            if (element.getType() != type || Element<T>::getType() != type) {
                return false;
            }

            // Update added selects to have the correct number of options
            if constexpr (std::is_same<T, SingleSelectContainer>::value) {
                element.getValueReference().update(SelectOptionsModification(OPTION_MODIFICATION_COUNT_UPDATE).getUpdateInfo(),
                                                   selectOptions.getOptionCount());
            } else if constexpr (std::is_same<T, SelectContainer>::value) {
                element.getValueReference().update(SelectOptionsModification(OPTION_MODIFICATION_COUNT_UPDATE).getUpdateInfo(),
                                                   selectOptions.getOptionCount());
            }

            m_rows.insert(m_rows.begin() + index, std::make_shared<Element<T>>(element));
            return true;
        }

        // Atempt to completely replace an element with a new one.
        // Internally: resets the shared_ptr for that element to the new value.
        template <typename T>
        bool replaceElement(size_t index, Element<T> newElement) {
            if (hasElement(index) == false) {
                return false;
            }
            if (newElement.getType() != type || Element<T>::getType() != type) {
                return false;
            }

            m_rows.at(index).reset(new Element<T>(newElement));
            return true;
        }

        bool removeElement(size_t index);

        template <typename T>
        T getElementValue(size_t index) const {
            if (hasElement(index) == false) {
                throw std::out_of_range(
                    std::format("Column::getElementValue(): The column {} has no element at index {}", name.c_str(), index));
            }
            auto typeElementPtr = std::dynamic_pointer_cast<const Element<T>>(m_rows[index]);
            return typeElementPtr->getValue();
        }

        std::weak_ptr<ElementBase> getElement(size_t index);
        std::weak_ptr<const ElementBase> getElementConst(size_t index) const;

        // SORT
        std::vector<size_t> getSortedIndices() const;

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

template <typename T>
class ColumnSortComparison {
    private:
        COLUMN_SORT m_sortDirection;

    public:
        ColumnSortComparison(COLUMN_SORT sort) : m_sortDirection(sort) {
        }

        bool operator()(std::shared_ptr<const ElementBase> left, std::shared_ptr<const ElementBase> right) {
            std::shared_ptr<const Element<T>> leftOfType = std::dynamic_pointer_cast<const Element<T>>(left);
            std::shared_ptr<const Element<T>> rightOfType = std::dynamic_pointer_cast<const Element<T>>(right);
            return m_sortDirection == COLUMN_SORT_DESCENDING ? leftOfType->getValue() > rightOfType->getValue()
                                                             : leftOfType->getValue() < rightOfType->getValue();
        }
};