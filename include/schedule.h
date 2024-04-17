#pragma once
#include "element_base.h"
#include "select_container.h"
#include <functional>
#include <vector>
#include <map>
#include <cstddef>
#include <string>
#include <element.h>
#include <schedule_edit.h>
#include <schedule_edit_history.h>
#include <input.h>
#include <schedule_column.h>
#include <schedule_core.h>

// TEMP
#include <iostream>

class Schedule
{
    private:
        ScheduleEditHistory m_editHistory;
        ScheduleCore m_core;

        // input listeners TODO: move to schedule_edit ?
        std::function<void()> undoCallback = std::function<void()>([&]()
        {
            undo();
        });
        std::function<void()> redoCallback = std::function<void()>([&]()
        {
            redo();
        });

    public:
        const std::map<SCHEDULE_TYPE, const char*> scheduleTypeNames =
        {
            {SCH_BOOL, "Checkbox"},
            {SCH_NUMBER, "Number"},
            {SCH_DECIMAL, "Decimal"},
            {SCH_TEXT, "Text"},
            {SCH_SELECT, "Select"},
            {SCH_WEEKDAY, "Select Weekday"},
            {SCH_TIME, "Time"},
            {SCH_DATE, "Date"},
        };

        // WHOLE-SCHEDULE FUNCTIONS
        void init(Input& input);

        // Clear the current Schedule and replace it with an empty Schedule with default Columns.
        void createDefaultSchedule();
        // Set the schedule's name to the provided name. NOTE: Does not affect filename. Only called by IO_Manager and MainMenuBarGui through IO_Manager.
        void setScheduleName(const std::string& name);
        std::string getScheduleName();

        // Clears the Schedule and deletes all the Columns.
        void clearSchedule();
        // Replaces the m_schedule vector of Columns with the provided. NOTE: ALSO DELETES ALL PREVIOUS ELEMENTS
        void replaceSchedule(std::vector<Column>& columns);

        const ScheduleEditHistory& getScheduleEditHistory();
        ScheduleEditHistory& getScheduleEditHistoryMut();

        // Get a constant pointer to the Column at the index. TODO: Return const ref instead
        const Column* getColumn(size_t column);
        // Get a constant reference to every Column in the Schedule (m_schedule)
        const std::vector<Column>& getColumns();
        // DO NOT USE UNLESS MEANING TO OVERWRITE THE ENTIRE SCHEDULE!
        std::vector<Column>& getMutableColumns();
        const SelectOptions& getColumnSelectOptions(size_t column);
        // NOTE: For OPTION_MODIFICATION_ADD the first string in optionName is used as the name.
        void modifyColumnSelectOptions(size_t column, OPTION_MODIFICATION selectModification, size_t firstIndex = 0, size_t secondIndex = 0, const std::vector<std::string>& optionNames = std::vector<std::string>{}, bool addToHistory = true);
        void setColumnType(size_t column, SCHEDULE_TYPE type, bool addToHistory = true);
        void setColumnName(size_t column, const char* name, bool addToHistory = true);
        void setColumnSort(size_t column, COLUMN_SORT sortDirection, bool addToHistory = true);

        size_t getColumnCount();
        size_t getRowCount();

        void sortColumns();

        // Sets every Element in the Column index to a default value of the given type. Do NOT change the column's type before running this. The Column type should only be changed after every row of it IS that type.
        void resetColumn(size_t index, SCHEDULE_TYPE type);
        void addRow(size_t index, bool addToHistory = true);
        void removeRow(size_t index, bool addToHistory = true);
        // Set all elements of a row. NOTE: The element data must be in the correct order. If the row doesn't exist, nothing happens.
        void setRow(size_t index, std::vector<ElementBase*> elementData);
        // Get all elements of a row. If the row doesn't exist, an empty vector is returned.
        std::vector<ElementBase*> getRow(size_t index);
        void addDefaultColumn(size_t index, bool addToHistory = true);
        void addColumn(size_t index, const Column& column, bool addToHistory = true);
        void removeColumn(size_t column, bool addToHistory = true);

        void undo();
        void redo();

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
            m_core.setElement(column, row, other, resort);
            
            m_editHistory.setEditedSinceWrite(true);
        }

        // Shortcut for getting the value of an Element at column; row
        template <typename T>
        T getElementValue(size_t column, size_t row)
        {
            return m_core.getElementValue<T>(column, row);
        }

        // Shortcut for setting the value of the Element at column; row to value. You must provide the correct type for the Element.
        template <typename T>
        void setElementValue(size_t column, size_t row, const T& value, bool resort = true, bool addToHistory = true)
        {
            ElementBase* element = m_core.getElement(column, row);

            // add the edit to history
            if (addToHistory)
            {
                m_editHistory.addEdit(new ElementEdit<T>(column, row, element->getType(), ((Element<T>*)element)->getValue(), value)); 
            }

            m_core.setElementValue<T>(column, row, value, resort);

            m_editHistory.setEditedSinceWrite(true);
        }
};