#pragma once

#include "filters/filter_rule.h"
#include "element_base.h"
#include "element.h"
#include "schedule_column.h"
#include "schedule_core.h"

enum class ScheduleEditType {
    ElementChange,
    RowAddOrRemove,
    ColumnAddOrRemove,
    ColumnPropertyChange,
    ColumnReset,
    FilterGroupAddOrRemove,
    FilterGroupChange,
    FilterAddOrRemove,
    FilterChange,
    FilterRuleAddOrRemove,
    FilterRuleChange
};

enum COLUMN_PROPERTY {
    COLUMN_PROPERTY_NAME,
    COLUMN_PROPERTY_TYPE,
    COLUMN_PROPERTY_SELECT_OPTIONS,
    COLUMN_PROPERTY_SORT,
    COLUMN_PROPERTY_RESET_OPTION,
};

class ScheduleEdit {
    protected:
        bool m_isReverted = false;
        ScheduleEditType m_type;

    public:
        ScheduleEdit(ScheduleEditType type);
        virtual ~ScheduleEdit();
        virtual void revert(ScheduleCore& scheduleCore);
        virtual void apply(ScheduleCore& scheduleCore);
        bool getIsReverted() const {
            return m_isReverted;
        }

        ScheduleEditType getType() const {
            return m_type;
        }
};

class ElementEditBase : public ScheduleEdit {
    protected:
        SCHEDULE_TYPE m_elementType;
        size_t m_column, m_row;

    public:
        ElementEditBase(size_t column, size_t row, SCHEDULE_TYPE elementType);

        std::pair<size_t, size_t> getPosition() const {
            return std::pair<size_t, size_t>(m_row, m_column);
        }

        SCHEDULE_TYPE getElementType() const {
            return m_elementType;
        }
};

template <typename T>
class ElementEdit : public ElementEditBase {
    private:
        T m_previousValue;
        T m_newValue;

    public:
        ElementEdit(size_t column, size_t row, SCHEDULE_TYPE elementType, const T& previousValue, const T& newValue)
            : ElementEditBase(column, row, elementType) {
            m_previousValue = previousValue;
            m_newValue = newValue;
        }

        void revert(ScheduleCore& scheduleCore) override {
            scheduleCore.setElementValue(m_column, m_row, m_previousValue, true);
            m_isReverted = true;
        }

        void apply(ScheduleCore& scheduleCore) override {
            scheduleCore.setElementValue(m_column, m_row, m_newValue, true);
            m_isReverted = false;
        }
};

class RowEdit : public ScheduleEdit {
    private:
        bool m_isRemove = false;
        size_t m_row;
        std::vector<ElementBase*> m_elementData = {};

    public:
        RowEdit(bool isRemove, size_t row, const std::vector<ElementBase*>& elementDataToCopy);

        ~RowEdit() override;

        void revert(ScheduleCore& scheduleCore) override;

        void apply(ScheduleCore& scheduleCore) override;

        bool getIsRemove() const {
            return m_isRemove;
        }

        size_t getRow() const {
            return m_row;
        }
};

class ColumnEdit : public ScheduleEdit {
    private:
        bool m_isRemove = false;
        size_t m_column;
        Column m_columnData;

    public:
        ColumnEdit(bool isRemove, size_t column, const Column& columnData);

        void revert(ScheduleCore& scheduleCore) override;

        void apply(ScheduleCore& scheduleCore) override;

        bool getIsRemove() const {
            return m_isRemove;
        }

        size_t getColumn() const {
            return m_column;
        }

        const Column& getColumnData() const {
            return m_columnData;
        }
};

class ColumnPropertyEdit : public ScheduleEdit {
    private:
        size_t m_column;
        Column m_columnData;
        Column m_previousColumnData;
        COLUMN_PROPERTY m_editedProperty;

    public:
        ColumnPropertyEdit(size_t column, COLUMN_PROPERTY editedProperty, const Column& previousData, const Column& newData);

        void revert(ScheduleCore& scheduleCore) override;

        void apply(ScheduleCore& scheduleCore) override;

        size_t getColumn() const {
            return m_column;
        }

        COLUMN_PROPERTY getEditedProperty() const {
            return m_editedProperty;
        }

        // Returns the Column name from the previous data
        std::string getColumnName() const;
};

class ColumnResetEdit : public ScheduleEdit {
    private:
        size_t m_column;
        Column m_columnData;

    public:
        ColumnResetEdit(size_t column, const Column& columnData);

        void revert(ScheduleCore& scheduleCore) override;

        void apply(ScheduleCore& scheduleCore) override;

        size_t getColumn() const {
            return m_column;
        }

        const Column& getColumnData() const {
            return m_columnData;
        }
};

class FilterEditBase : public ScheduleEdit {
    protected:
        size_t m_columnIndex = 0;
        size_t m_filterGroupIndex = 0;
        size_t m_filterIndex = 0;
        size_t m_filterRuleIndex = 0;

    public:
        FilterEditBase(ScheduleEditType editType,
                       size_t columnIndex,
                       size_t filterGroupIndex,
                       size_t filterIndex = 0,
                       size_t filterRuleIndex = 0);

        size_t getColumnIndex() const {
            return m_columnIndex;
        }

        size_t getFilterGroupIndex() const {
            return m_filterGroupIndex;
        }
};

class FilterGroupAddOrRemoveEdit : public FilterEditBase {
    private:
        bool m_isRemove = false;
        FilterGroup m_filterGroup;

    public:
        FilterGroupAddOrRemoveEdit(bool isRemove, size_t column, size_t filterGroupIndex, const FilterGroup& filterGroup);

        bool getIsRemove() const {
            return m_isRemove;
        }

        // NOT sure if this is actually const.. you could possibly get this FilterGroup and then modify one of its rules, which would affect all other FilterGroups with that rule.
        // but shouldn't undo / redo mostly take care of that? we will see.
        FilterGroup getFilterGroup() const {
            return m_filterGroup;
        }

        void revert(ScheduleCore& scheduleCore) override {
            // reverting a removal means adding
            if (m_isRemove) {
                scheduleCore.addColumnFilterGroup(m_columnIndex, m_filterGroupIndex, m_filterGroup);
            }
            // reverting an addition means removing
            else
            {
                scheduleCore.removeColumnFilterGroup(m_columnIndex, m_filterGroupIndex);
            }

            m_isReverted = true;
        }

        void apply(ScheduleCore& scheduleCore) override {
            // applying a removal means removing
            if (m_isRemove) {
                scheduleCore.removeColumnFilterGroup(m_columnIndex, m_filterGroupIndex);
            }
            // applying an addition means adding
            else
            {
                scheduleCore.addColumnFilterGroup(m_columnIndex, m_filterGroupIndex, m_filterGroup);
            }

            m_isReverted = false;
        }
};

class FilterGroupChangeEdit : public FilterEditBase {
    private:
        LogicalOperatorEnum m_previousOperator;
        std::string m_previousName;
        bool m_previousEnabled;
        LogicalOperatorEnum m_newOperator;
        std::string m_newName;
        bool m_newEnabled;

    public:
        FilterGroupChangeEdit(size_t column,
                              size_t filterGroupIndex,
                              LogicalOperatorEnum prevOperator,
                              const std::string& prevName,
                              bool prevEnabled,
                              const FilterGroup& current);

        size_t getFilterIndex() const {
            return m_filterIndex;
        }

        LogicalOperatorEnum getPrevOperator() const {
            return m_previousOperator;
        }

        std::string getPrevName() const {
            return m_previousName;
        }

        bool getPrevEnabled() const {
            return m_previousEnabled;
        }

        LogicalOperatorEnum getNewOperator() const {
            return m_newOperator;
        }

        std::string getNewName() const {
            return m_newName;
        }

        bool getNewEnabled() const {
            return m_newEnabled;
        }

        void revert(ScheduleCore& scheduleCore) override {
            scheduleCore.setColumnFilterGroupOperator(m_columnIndex, m_filterGroupIndex, m_previousOperator);
            scheduleCore.setColumnFilterGroupName(m_columnIndex, m_filterGroupIndex, m_previousName);
            scheduleCore.setColumnFilterGroupEnabled(m_columnIndex, m_filterGroupIndex, m_previousEnabled);
            m_isReverted = true;
        }

        void apply(ScheduleCore& scheduleCore) override {
            scheduleCore.setColumnFilterGroupOperator(m_columnIndex, m_filterGroupIndex, m_newOperator);
            scheduleCore.setColumnFilterGroupName(m_columnIndex, m_filterGroupIndex, m_newName);
            scheduleCore.setColumnFilterGroupEnabled(m_columnIndex, m_filterGroupIndex, m_newEnabled);
            m_isReverted = false;
        }
};

class FilterAddOrRemoveEdit : public FilterEditBase {
    private:
        bool m_isRemove = false;
        Filter m_filter;

    public:
        FilterAddOrRemoveEdit(bool isRemove, size_t column, size_t filterGroupIndex, size_t filterIndex, const Filter& filter);

        bool getIsRemove() const {
            return m_isRemove;
        }

        size_t getFilterIndex() const {
            return m_filterIndex;
        }

        // NOT sure if this is actually const.. you could possibly get this Filter and then modify one of its rules, which would affect all other Filters with that rule.
        // but shouldn't undo / redo mostly take care of that? we will see.
        Filter getFilter() const {
            return m_filter;
        }

        void revert(ScheduleCore& scheduleCore) override {
            // reverting a removal means adding
            if (m_isRemove) {
                scheduleCore.addColumnFilter(m_columnIndex, m_filterGroupIndex, m_filterIndex, m_filter);
            }
            // reverting an addition means removing
            else
            {
                scheduleCore.removeColumnFilter(m_columnIndex, m_filterGroupIndex, m_filterIndex);
            }

            m_isReverted = true;
        }

        void apply(ScheduleCore& scheduleCore) override {
            // applying a removal means removing
            if (m_isRemove) {
                scheduleCore.removeColumnFilter(m_columnIndex, m_filterGroupIndex, m_filterIndex);
            }
            // applying an addition means adding
            else
            {
                scheduleCore.addColumnFilter(m_columnIndex, m_filterGroupIndex, m_filterIndex, m_filter);
            }

            m_isReverted = false;
        }
};

class FilterChangeEdit : public FilterEditBase {
    private:
        LogicalOperatorEnum m_previousOperator;
        LogicalOperatorEnum m_newOperator;

    public:
        FilterChangeEdit(size_t column,
                         size_t filterGroupIndex,
                         size_t filterIndex,
                         LogicalOperatorEnum previousOperator,
                         LogicalOperatorEnum newOperator);

        size_t getFilterIndex() const {
            return m_filterIndex;
        }

        LogicalOperatorEnum getPrevOperator() const {
            return m_previousOperator;
        }

        LogicalOperatorEnum getNewOperator() const {
            return m_newOperator;
        }

        void revert(ScheduleCore& scheduleCore) override {
            scheduleCore.setColumnFilterOperator(m_columnIndex, m_filterGroupIndex, m_filterIndex, m_previousOperator);
            m_isReverted = true;
        }

        void apply(ScheduleCore& scheduleCore) override {
            scheduleCore.setColumnFilterOperator(m_columnIndex, m_filterGroupIndex, m_filterIndex, m_newOperator);
            m_isReverted = false;
        }
};

class FilterRuleAddOrRemoveEditBase : public FilterEditBase {
    protected:
        bool m_isRemove = false;

    public:
        FilterRuleAddOrRemoveEditBase(
            bool isRemove, size_t column, size_t filterGroupIndex, size_t filterIndex, size_t filterRuleIndex);

        size_t getFilterIndex() const {
            return m_filterIndex;
        }

        size_t getFilterRuleIndex() const {
            return m_filterRuleIndex;
        }

        bool getIsRemove() const {
            return m_isRemove;
        }
};

template <typename T>
class FilterRuleAddOrRemoveEdit : public FilterRuleAddOrRemoveEditBase {
    private:
        FilterRule<T> m_filterRule = FilterRule<T>(T());

    public:
        FilterRuleAddOrRemoveEdit(bool isRemove,
                                  size_t columnIndex,
                                  size_t filterGroupIndex,
                                  size_t filterIndex,
                                  size_t filterRuleIndex,
                                  const FilterRule<T>& filterRule)
            : FilterRuleAddOrRemoveEditBase(isRemove, columnIndex, filterGroupIndex, filterIndex, filterRuleIndex) {
            m_filterRule = filterRule;
        }

        FilterRule<T> getRule() const {
            return m_filterRule;
        }

        void revert(ScheduleCore& scheduleCore) override {
            // reverting a removal means adding the rule
            if (m_isRemove) {
                scheduleCore.addColumnFilterRule(
                    m_columnIndex, m_filterGroupIndex, m_filterIndex, m_filterRuleIndex, m_filterRule);
            }
            // reverting an addition means removing the rule
            else
            {
                scheduleCore.removeColumnFilterRule(m_columnIndex, m_filterGroupIndex, m_filterIndex, m_filterRuleIndex);
            }

            m_isReverted = true;
        }

        void apply(ScheduleCore& scheduleCore) override {
            // applying a removal means removing the filter
            if (m_isRemove) {
                scheduleCore.removeColumnFilterRule(m_columnIndex, m_filterGroupIndex, m_filterIndex, m_filterRuleIndex);
            }
            // applying an addition means adding the filter
            else
            {
                scheduleCore.addColumnFilterRule(
                    m_columnIndex, m_filterGroupIndex, m_filterIndex, m_filterRuleIndex, m_filterRule);
            }

            m_isReverted = false;
        }
};

class FilterRuleChangeEditBase : public FilterEditBase {
    protected:
        FilterRuleContainer m_previousRule;
        FilterRuleContainer m_newRule;

    public:
        FilterRuleChangeEditBase(size_t column,
                                 size_t filterGroupIndex,
                                 size_t filterIndex,
                                 size_t filterRuleIndex,
                                 const FilterRuleContainer& previousValue,
                                 const FilterRuleContainer& newValue);

        size_t getFilterIndex() const {
            return m_filterIndex;
        }

        size_t getFilterRuleIndex() const {
            return m_filterRuleIndex;
        }

        FilterRuleContainer getPrevRule() const {
            return m_previousRule;
        }

        FilterRuleContainer getNewRule() const {
            return m_newRule;
        }
};

template <typename T>
class FilterRuleChangeEdit : public FilterRuleChangeEditBase {
    public:
        FilterRuleChangeEdit(size_t column,
                             size_t filterGroupIndex,
                             size_t filterIndex,
                             size_t filterRuleIndex,
                             const FilterRule<T>& previousValue,
                             const FilterRule<T>& newValue)
            : FilterRuleChangeEditBase(column, filterGroupIndex, filterIndex, filterRuleIndex, previousValue, newValue) {
        }

        void revert(ScheduleCore& scheduleCore) override {
            scheduleCore.replaceColumnFilterRule(
                m_columnIndex, m_filterGroupIndex, m_filterIndex, m_filterRuleIndex, m_previousRule.getAsType<T>());
            m_isReverted = true;
        }

        void apply(ScheduleCore& scheduleCore) override {
            scheduleCore.replaceColumnFilterRule(
                m_columnIndex, m_filterGroupIndex, m_filterIndex, m_filterRuleIndex, m_newRule.getAsType<T>());
            m_isReverted = false;
        }
};