#pragma once
#include <functional>
#include <vector>
#include <map>
#include <cstddef>
#include <string>
#include <element_base.h>
#include <element.h>
#include <schedule_edit.h>
#include <schedule_edit_history.h>
#include <input.h>
#include <interface.h>
#include <schedule_column.h>
#include <schedule_core.h>

// TEMP
#include <iostream>

const size_t SCHEDULE_NAME_MAX_LENGTH = 48;

class Schedule
{
    private:
        ScheduleEditHistory m_editHistory;
        ScheduleCore m_core;
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
        std::function<void(const size_t&, const SelectOptionsModification&)> modifyColumnSelectOptionsListener = [&](const size_t& i, const SelectOptionsModification& modification)
        {
            modifyColumnSelectOptions(i, modification);
        };

        // FilterEditorSubGui
        std::function<void(const size_t&, const std::shared_ptr<FilterBase>&)> addFilterListener = [&](const size_t& col, const std::shared_ptr<FilterBase>& filter)
        {
            addColumnFilter(col, filter);
        };
        std::function<void(const size_t&, const size_t&, const std::shared_ptr<FilterBase>&)> editFilterListener = [&](const size_t& col, const size_t& filterIndex, const std::shared_ptr<FilterBase>& filter)
        {
            replaceColumnFilter(col, filterIndex, filter);
        };
        std::function<void(const size_t&, const size_t&)> removeFilterListener = [&](const size_t& col, const size_t& filterIndex)
        {
            removeColumnFilter(col, filterIndex);
        };

        // setElementValue HELL (ScheduleGui)
        std::function<void(const size_t&, const size_t&, const bool&)> setElementValueListenerBool = [&](const size_t& col, const size_t& row, const bool& val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(const size_t&, const size_t&, const int&)> setElementValueListenerNumber = [&](const size_t& col, const size_t& row, const int& val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(const size_t&, const size_t&, const double&)> setElementValueListenerDecimal = [&](const size_t& col, const size_t& row, const double& val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(const size_t&, const size_t&, const std::string&)> setElementValueListenerText = [&](const size_t& col, const size_t& row, const std::string& val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(const size_t&, const size_t&, const SelectContainer&)> setElementValueListenerSelect = [&](const size_t& col, const size_t& row, const SelectContainer& val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(const size_t&, const size_t&, const TimeContainer&)> setElementValueListenerTime = [&](const size_t& col, const size_t& row, const TimeContainer& val)
        {
            setElementValue(col, row, val);
        };
        std::function<void(const size_t&, const size_t&, const DateContainer&)> setElementValueListenerDate = [&](const size_t& col, const size_t& row, const DateContainer& val)
        {
            setElementValue(col, row, val);
        };
        // column add / remove
        std::function<void(const size_t&)> addDefaultColumnListener = [&](const size_t& col)
        {
            addDefaultColumn(col);
        };
        std::function<void(const size_t&)> removeColumnListener = [&](const size_t& col)
        {
            removeColumn(col);
        };
        // column modification
        std::function<void(const size_t&, const SCHEDULE_TYPE&)> setColumnTypeListener = [&](const size_t& col, const SCHEDULE_TYPE& type)
        {
            setColumnType(col, type);
        };
        std::function<void(const size_t&, const COLUMN_SORT&)> setColumnSortListener = [&](const size_t& col, const COLUMN_SORT& sort)
        {
            setColumnSort(col, sort);
        };
        std::function<void(const size_t&, const std::string&)> setColumnNameListener = [&](const size_t& col, const std::string& name)
        {
            setColumnName(col, name);
        };
        // row modification
        std::function<void(const size_t&)> addRowListener = [&](const size_t& col)
        {
            addRow(col);
        };
        std::function<void(const size_t&)> removeRowListener = [&](const size_t& col)
        {
            removeRow(col);
        };

    public:
        // WHOLE-SCHEDULE FUNCTIONS
        void init(Input& input, Interface& interface);

        // Set the schedule's name to the provided name. NOTE: Does not affect filename. Only called by IO_Manager and MainMenuBarGui through IO_Manager.
        void setName(const std::string& name);
        std::string getName();
        const ScheduleEditHistory& getEditHistory();
        ScheduleEditHistory& getEditHistoryMutable();
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
        const SelectOptions& getColumnSelectOptions(size_t column);
        // NOTE: For OPTION_MODIFICATION_ADD the first string in optionName is used as the name.
        void modifyColumnSelectOptions(size_t column, const SelectOptionsModification& selectOptionsModification, bool addToHistory = true);
        void addColumnFilter(size_t column, const std::shared_ptr<FilterBase>& filter, bool addToHistory = true);
        void replaceColumnFilter(size_t column, size_t index, const std::shared_ptr<FilterBase>& filter);
        void removeColumnFilter(size_t column, size_t index, bool addToHistory = true);
        // Sets every Element in the Column index to a default value of the given type. Do NOT change the column's type before running this. The Column type should only be changed after every row of it IS that type.
        void resetColumn(size_t index, SCHEDULE_TYPE type);

        // ROWS
        size_t getRowCount();
        void addRow(size_t index, bool addToHistory = true);
        void removeRow(size_t index, bool addToHistory = true);
        // Get all elements of a row. If the row doesn't exist, an empty vector is returned.
        std::vector<ElementBase*> getRow(size_t index);
        // Set all elements of a row. NOTE: The element data must be in the correct order. If the row doesn't exist, nothing happens.
        void setRow(size_t index, std::vector<ElementBase*> elementData);
        std::vector<size_t> getSortedRowIndices();

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
                m_editHistory.addEdit(new ElementEdit<T>(column, row, element->getType(), ((Element<T>*)element)->getValue(), value)); 
            }

            m_core.setElementValue<T>(column, row, value, resort);

            m_editHistory.setEditedSinceWrite(true);
        }
};