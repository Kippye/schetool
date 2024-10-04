#pragma once
#include <functional>
#include <vector>
#include <map>
#include <cstddef>
#include <string>
#include "schedule_events.h"
#include "element_base.h"
#include "element.h"
#include "schedule_edit.h"
#include "schedule_edit_history.h"
#include "input.h"
#include "interface.h"
#include "schedule_column.h"
#include "schedule_core.h"
#include "schedule_gui.h"

const size_t SCHEDULE_NAME_MAX_LENGTH = 48;

class Schedule
{
    private:
        ScheduleEditHistory m_editHistory;
        ScheduleCore m_core;
        ScheduleEvents m_scheduleEvents;
        std::shared_ptr<ScheduleGui> m_scheduleGui;
        std::string m_scheduleName;

        // input listeners AND gui listeners
        std::function<void()> undoListener = std::function<void()>([&]()
        {
            undo();
        });
        std::function<void()> redoListener = std::function<void()>([&]()
        {
            redo();
        });

        // modifyColumnSelectOptions (ElementEditorSubGui)
        std::function<void(size_t, SelectOptionsModification)> modifyColumnSelectOptionsListener = [&](size_t i, SelectOptionsModification modification)
        {
            modifyColumnSelectOptions(i, modification);
        };

        // FilterEditorSubGui
        std::function<void(size_t, FilterGroup)> addFilterGroupListener = [&](size_t col, FilterGroup filterGroup)
        {
            addColumnFilterGroup(col, filterGroup);
        };
        std::function<void(size_t, size_t, std::string)> setFilterGroupNameListener = [&](size_t col, size_t groupIndex, std::string name)
        {
            setColumnFilterGroupName(col, groupIndex, name);
        };
        std::function<void(size_t, size_t, LogicalOperatorEnum)> setFilterGroupOperatorListener = [&](size_t col, size_t groupIndex, LogicalOperatorEnum logicalOperator)
        {
            setColumnFilterGroupOperator(col, groupIndex, logicalOperator);
        };
        std::function<void(size_t, size_t, bool)> setFilterGroupEnabledListener = [&](size_t col, size_t groupIndex, bool enabled)
        {
            setColumnFilterGroupEnabled(col, groupIndex, enabled);
        };
        std::function<void(size_t, size_t)> removeFilterGroupListener = [&](size_t col, size_t groupIndex)
        {
            removeColumnFilterGroup(col, groupIndex);
        };

        std::function<void(size_t, size_t, Filter)> addFilterListener = [&](size_t col, size_t groupIndex, Filter filter)
        {
            addColumnFilter(col, groupIndex, filter);
        };
        std::function<void(size_t, size_t, size_t, LogicalOperatorEnum)> setFilterOperatorListener = [&](size_t col, size_t groupIndex, size_t filterIndex, LogicalOperatorEnum logicalOperator)
        {
            setColumnFilterOperator(col, groupIndex, filterIndex, logicalOperator);
        };
        std::function<void(size_t, size_t, size_t)> removeFilterListener = [&](size_t col, size_t groupIndex, size_t filterIndex)
        {
            removeColumnFilter(col, groupIndex, filterIndex);
        };

        std::function<void(size_t, size_t, size_t, FilterRuleContainer)> addFilterRuleListener = [&](size_t col, size_t groupIndex, size_t filterIndex, FilterRuleContainer filterRule)
        {
            SCHEDULE_TYPE columnType = getColumn(col)->type;
            switch(columnType)
            {
                case SCH_BOOL:
                    addColumnFilterRule<bool>(col, groupIndex, filterIndex, filterRule.getAsType<bool>());
                break;
                case SCH_NUMBER:
                    addColumnFilterRule<int>(col, groupIndex, filterIndex, filterRule.getAsType<int>());
                break;
                case SCH_DECIMAL:
                    addColumnFilterRule<double>(col, groupIndex, filterIndex, filterRule.getAsType<double>());
                break;
                case SCH_TEXT:
                    addColumnFilterRule<std::string>(col, groupIndex, filterIndex, filterRule.getAsType<std::string>());
                break;
                case SCH_SELECT:
                    addColumnFilterRule<SingleSelectContainer>(col, groupIndex, filterIndex, filterRule.getAsType<SingleSelectContainer>());
                break;
                case SCH_MULTISELECT:
                    addColumnFilterRule<SelectContainer>(col, groupIndex, filterIndex, filterRule.getAsType<SelectContainer>());
                break;
                case SCH_WEEKDAY:
                    addColumnFilterRule<WeekdayContainer>(col, groupIndex, filterIndex, filterRule.getAsType<WeekdayContainer>());
                break;
                case SCH_TIME:
                    addColumnFilterRule<TimeContainer>(col, groupIndex, filterIndex, filterRule.getAsType<TimeContainer>());
                break;
                case SCH_DATE:
                    addColumnFilterRule<DateContainer>(col, groupIndex, filterIndex, filterRule.getAsType<DateContainer>());
                break;
                default:
                    printf("Schedule listener addFilterRuleListener(): Failed to find Schedule::addColumnFilterRule() for type %d\n", columnType);
                break;
            }
        };
        std::function<void(size_t, size_t, size_t, size_t, FilterRuleContainer, FilterRuleContainer)> editFilterRuleListener = [&](size_t col, size_t groupIndex, size_t filterIndex, size_t ruleIndex, FilterRuleContainer previousRule, FilterRuleContainer rule)
        {
            SCHEDULE_TYPE columnType = getColumn(col)->type;
            switch(columnType)
            {
                case SCH_BOOL:
                    replaceColumnFilterRule<bool>(col, groupIndex, filterIndex, ruleIndex,  previousRule.getAsType<bool>(), rule.getAsType<bool>());
                break;
                case SCH_NUMBER:
                    replaceColumnFilterRule<int>(col, groupIndex, filterIndex, ruleIndex,  previousRule.getAsType<int>(), rule.getAsType<int>());
                break;
                case SCH_DECIMAL:
                    replaceColumnFilterRule<double>(col, groupIndex, filterIndex, ruleIndex,  previousRule.getAsType<double>(), rule.getAsType<double>());
                break;
                case SCH_TEXT:
                    replaceColumnFilterRule<std::string>(col, groupIndex, filterIndex, ruleIndex,  previousRule.getAsType<std::string>(), rule.getAsType<std::string>());
                break;
                case SCH_SELECT:
                    replaceColumnFilterRule<SingleSelectContainer>(col, groupIndex, filterIndex, ruleIndex,  previousRule.getAsType<SingleSelectContainer>(), rule.getAsType<SingleSelectContainer>());
                break;
                case SCH_MULTISELECT:
                    replaceColumnFilterRule<SelectContainer>(col, groupIndex, filterIndex, ruleIndex,  previousRule.getAsType<SelectContainer>(), rule.getAsType<SelectContainer>());
                break;
                case SCH_WEEKDAY:
                    replaceColumnFilterRule<WeekdayContainer>(col, groupIndex, filterIndex, ruleIndex,  previousRule.getAsType<WeekdayContainer>(), rule.getAsType<WeekdayContainer>());
                break;
                case SCH_TIME:
                    replaceColumnFilterRule<TimeContainer>(col, groupIndex, filterIndex, ruleIndex,  previousRule.getAsType<TimeContainer>(), rule.getAsType<TimeContainer>());
                break;
                case SCH_DATE:
                    replaceColumnFilterRule<DateContainer>(col, groupIndex, filterIndex, ruleIndex,  previousRule.getAsType<DateContainer>(), rule.getAsType<DateContainer>());
                break;
                default:
                    printf("Schedule listener editFilterRuleListener(): Failed to find Schedule::replaceColumnFilterRule() for type %d\n", columnType);
                break;
            }
        };
        std::function<void(size_t, size_t, size_t, size_t)> removeFilterRuleListener = [&](size_t col, size_t groupIndex, size_t filterIndex, size_t ruleIndex)
        {
            SCHEDULE_TYPE columnType = getColumn(col)->type;
            switch(columnType)
            {
                case SCH_BOOL:
                    removeColumnFilterRule<bool>(col, groupIndex, filterIndex, ruleIndex);
                break;
                case SCH_NUMBER:
                    removeColumnFilterRule<int>(col, groupIndex, filterIndex, ruleIndex);
                break;
                case SCH_DECIMAL:
                    removeColumnFilterRule<double>(col, groupIndex, filterIndex, ruleIndex);
                break;
                case SCH_TEXT:
                    removeColumnFilterRule<std::string>(col, groupIndex, filterIndex, ruleIndex);
                break;
                case SCH_SELECT:
                    removeColumnFilterRule<SingleSelectContainer>(col, groupIndex, filterIndex, ruleIndex);
                break;
                case SCH_MULTISELECT:
                    removeColumnFilterRule<SelectContainer>(col, groupIndex, filterIndex, ruleIndex);
                break;
                case SCH_WEEKDAY:
                    removeColumnFilterRule<WeekdayContainer>(col, groupIndex, filterIndex, ruleIndex);
                break;
                case SCH_TIME:
                    removeColumnFilterRule<TimeContainer>(col, groupIndex, filterIndex, ruleIndex);
                break;
                case SCH_DATE:
                    removeColumnFilterRule<DateContainer>(col, groupIndex, filterIndex, ruleIndex);
                break;
                default:
                    printf("Schedule listener removeFilterRuleListener(): Failed to find Schedule::removeColumnFilterRule() for type %d\n", columnType);
                break;
            }
        };

        // setElementValue HELL (ScheduleGui)
        std::function<void(size_t, size_t, bool)> setElementValueListenerBool = [&](size_t col, size_t row, bool val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(size_t, size_t, int)> setElementValueListenerNumber = [&](size_t col, size_t row, int val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(size_t, size_t, double)> setElementValueListenerDecimal = [&](size_t col, size_t row, double val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(size_t, size_t, std::string)> setElementValueListenerText = [&](size_t col, size_t row, std::string val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(size_t, size_t, SelectContainer)> setElementValueListenerSelect = [&](size_t col, size_t row, SelectContainer val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(size_t, size_t, WeekdayContainer)> setElementValueListenerWeekday = [&](size_t col, size_t row, WeekdayContainer val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(size_t, size_t, TimeContainer)> setElementValueListenerTime = [&](size_t col, size_t row, TimeContainer val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(size_t, size_t, DateContainer)> setElementValueListenerDate = [&](size_t col, size_t row, DateContainer val)
        {
            setElementValue(col, row, val);
        };
        // column add / remove
        std::function<void(size_t)> addDefaultColumnListener = [&](size_t col)
        {
            addDefaultColumn(col);
        };
        std::function<void(size_t)> removeColumnListener = [&](size_t col)
        {
            removeColumn(col);
        };
        // column modification
        std::function<void(size_t, SCHEDULE_TYPE)> setColumnTypeListener = [&](size_t col, SCHEDULE_TYPE type)
        {
            setColumnType(col, type);
        };
        std::function<void(size_t, COLUMN_SORT)> setColumnSortListener = [&](size_t col, COLUMN_SORT sort)
        {
            setColumnSort(col, sort);
        };
        std::function<void(size_t, std::string)> setColumnNameListener = [&](size_t col, std::string name)
        {
            setColumnName(col, name);
        };
        std::function<void(size_t, ColumnResetOption)> setColumnResetOptionListener = [&](size_t col, ColumnResetOption option)
        {
            setColumnResetOption(col, option);
        };
        // whole column modification
        std::function<void(size_t, bool)> resetColumnListener = [&](size_t col, bool addToHistory)
        {
            resetColumn(col, addToHistory);
        };
        // row modification
        std::function<void(size_t)> addRowListener = [&](size_t col)
        {
            addRow(col);
        };
        std::function<void(size_t)> removeRowListener = [&](size_t col)
        {
            removeRow(col);
        };

    public:
        Schedule();
        // WHOLE-SCHEDULE FUNCTIONS
        void init(Input& input, Interface& interface);

        // Set the schedule's name to the provided name. NOTE: Does not affect filename. Only called by IO_Manager and MainMenuBarGui through IO_Manager.
        void setName(const std::string& name);
        std::string getName();
        const ScheduleEditHistory& getEditHistory();
        ScheduleEditHistory& getEditHistoryMutable();
        ScheduleEvents& getScheduleEvents();
        void undo();
        void redo();
        // Clear the current Schedule and replace it with default Columns and no rows.
        void createDefaultSchedule();

        /// CORE WRAPPERS
        // Clears the Schedule and deletes all the Columns.
        void clearSchedule();
        // Replaces the vector of Columns with the provided. NOTE: ALSO DELETES ALL PREVIOUS ELEMENTS
        void replaceSchedule(std::vector<Column>& columns);
        // Get a constant reference to every Column in the Schedule
        const std::vector<Column>& getAllColumns();
        // Generally do not use this. It's meant for reading from file only.
        std::vector<Column>& getAllColumnsMutable();
        void sortColumns();

        // COLUMNS
        size_t getColumnCount();
        void addColumn(size_t index, const Column& column, bool addToHistory = true);
        void addDefaultColumn(size_t index, bool addToHistory = true);
        void removeColumn(size_t column, bool addToHistory = true);
        // Get a constant pointer to the Column at the index.
        const Column* getColumn(size_t column);
        void setColumnType(size_t column, SCHEDULE_TYPE type, bool addToHistory = true);
        void setColumnName(size_t column, const std::string& name, bool addToHistory = true);
        void setColumnSort(size_t column, COLUMN_SORT sortDirection, bool addToHistory = true);
        void setColumnResetOption(size_t column, ColumnResetOption option, bool addToHistory = true);
        const SelectOptions& getColumnSelectOptions(size_t column);
        // NOTE: For OPTION_MODIFICATION_ADD the first string in optionName is used as the name.
        void modifyColumnSelectOptions(size_t column, const SelectOptionsModification& selectOptionsModification, bool addToHistory = true);

        void addColumnFilterGroup(size_t column, FilterGroup filterGroup, bool addToHistory = true);
        void removeColumnFilterGroup(size_t column, size_t groupIndex, bool addToHistory = true);
        void setColumnFilterGroupName(size_t column, size_t groupIndex, const std::string& name, bool addToHistory = true);
        void setColumnFilterGroupOperator(size_t column, size_t groupIndex, LogicalOperatorEnum logicalOperator, bool addToHistory = true);
        void setColumnFilterGroupEnabled(size_t column, size_t groupIndex, bool enabled, bool addToHistory = true);

        void addColumnFilter(size_t column, size_t groupIndex, Filter filter, bool addToHistory = true);
        void removeColumnFilter(size_t column, size_t groupIndex, size_t filterIndex, bool addToHistory = true);
        void setColumnFilterOperator(size_t column, size_t groupIndex, size_t filterIndex, LogicalOperatorEnum logicalOperator, bool addToHistory = true);

        template <typename T>
        void addColumnFilterRule(size_t column, size_t groupIndex, size_t filterIndex, FilterRule<T> filterRule, bool addToHistory = true)
        {
            if (m_core.addColumnFilterRule(column, groupIndex, filterIndex, filterRule))
            {
                if (addToHistory)
                {
                    size_t filterRuleIndex = m_core.getColumn(column)->getFilterGroupConst(groupIndex).getFilterConst(filterIndex).getRuleCount() - 1;
                    m_editHistory.addEdit<FilterRuleAddOrRemoveEdit<T>>(false, column, groupIndex, filterIndex, filterRuleIndex, filterRule);
                }
            }
        }
        template <typename T>
        void replaceColumnFilterRule(size_t column, size_t groupIndex, size_t filterIndex, size_t ruleIndex, FilterRule<T> previousFilterRule, FilterRule<T> filterRule, bool addToHistory = true)
        {
            if (m_core.replaceColumnFilterRule<T>(column, groupIndex, filterIndex, ruleIndex, filterRule))
            {
                if (addToHistory)
                {
                    m_editHistory.addEdit<FilterRuleChangeEdit<T>>(column, groupIndex, filterIndex, ruleIndex, previousFilterRule, filterRule);
                }
            }
        }
        template <typename T>
        void removeColumnFilterRule(size_t column, size_t groupIndex, size_t filterIndex, size_t ruleIndex, bool addToHistory = true)  
        {
            FilterRule<T> filterRule = m_core.getColumn(column)->getFilterGroupConst(groupIndex).getFilterConst(filterIndex).getRuleConst(ruleIndex).getAsType<T>();

            if (m_core.removeColumnFilterRule(column, groupIndex, filterIndex, ruleIndex))
            {
                if (addToHistory)
                {
                    m_editHistory.addEdit<FilterRuleAddOrRemoveEdit<T>>(true, column, groupIndex, filterIndex, ruleIndex, filterRule);
                }
            }
        }
        // Sets every Element in the Column index to a default value of the given type. Do NOT change the column's type before running this. The Column type should only be changed after every row of it IS that type.
        void resetColumn(size_t index, bool addToHistory);

        // ROWS
        size_t getRowCount();
        void addRow(size_t index, bool addToHistory = true);
        void removeRow(size_t index, bool addToHistory = true);
        // Get all elements of a row. If the row doesn't exist, an empty vector is returned.
        std::vector<ElementBase*> getRow(size_t index);
        // Set all elements of a row. NOTE: The element data must be in the correct order. If the row doesn't exist, nothing happens.
        void setRow(size_t index, std::vector<ElementBase*> elementData);
        std::vector<size_t> getSortedRowIndices();

        // Interface methods
        void applyColumnTimeBasedReset(size_t columnIndex);

        // ELEMENTS.
        // Get the value of the element as Element<T>. NOTE: You MUST provide the correct type.
        template <typename T>
        T getValue(ElementBase* element)
        {
            return m_core.getValue<T>(element);
        }

        // Get a pointer to the ElementBase at column; row
        ElementBase* getElement(size_t column, size_t row)
        {
            return m_core.getElement(column, row);
        }

        // Simple function that gets an ElementBase* at column; row and casts it to Element<T>*. In the future, this might check that the returned type is actually correct.
        template <typename T>
        Element<T>* getElementAsSpecial(size_t column, size_t row)
        {
            return m_core.getElementAsSpecial<T>(column, row);
        }

        // Use this function to completely replace the element at column; row with the ElementBase in value.
        // NOTE: If the types match, a copy is performed.
        // If the types do not match, the target element pointer is replaced by the value pointer!
        // NOTE: Currently, does not add to the edit history
        void setElement(size_t column, size_t row, ElementBase* other, bool resort = true)
        {
            if (m_core.setElement(column, row, other, resort))
            {
                m_editHistory.setEditedSinceWrite(true);
            }
        }

        // Shortcut for getting the value of an Element at column; row
        template <typename T>
        T getElementValue(size_t column, size_t row)
        {
            return m_core.getElementValue<T>(column, row);
        }

        // Shortcut for getting the value of an Element at column; row by const reference
        template <typename T>
        const T& getElementValueConstRef(size_t column, size_t row)
        {
            return m_core.getElementValue<T>(column, row);
        }

        // Shortcut for setting the value of the Element at column; row to value. You must provide the correct type for the Element.
        template <typename T>
        void setElementValue(size_t column, size_t row, const T& value, bool resort = true, bool addToHistory = true)
        {
            ElementBase* element = m_core.getElement(column, row);

            if (element == nullptr)
            {
                printf("Schedule::setElementValue failed to set element at %zu; %zu - element does not exist\n", column, row);
                return;
            }

            // TODO: this might add to edit history even if it fails in the core
            // add the edit to history
            if (addToHistory)
            {
                m_editHistory.addEdit<ElementEdit<T>>(column, row, element->getType(), ((Element<T>*)element)->getValue(), value); 
            }

            m_core.setElementValue<T>(column, row, value, resort);

            m_editHistory.setEditedSinceWrite(true);
        }
};