#pragma once

#include <map>
#include "gui.h"
#include "filter_rule.h"
#include "select_container.h"
#include "time_container.h"
#include "date_container.h"
#include "element_base.h"
#include "schedule_events.h"
#include "schedule_core.h"

class FilterRuleEditorState
{
    private:
        bool m_isValid = false;
        size_t m_filterIndex = 0;
        size_t m_filterRuleIndex = 0;
        SCHEDULE_TYPE m_type = SCH_LAST;
        FilterRuleContainer m_filterRule;
    public:
        template <typename T>
        void setup(SCHEDULE_TYPE type, size_t filterIndex, size_t filterRuleIndex, FilterRule<T> filterRule)
        {
            m_type = type;
            m_filterIndex = filterIndex;
            m_filterRuleIndex = filterRuleIndex;
            m_filterRule = FilterRuleContainer().fill(filterRule);
            m_isValid = true;
        }
        SCHEDULE_TYPE getType() const;
        std::pair<size_t, size_t> getIndices() const;
        FilterRuleContainer& getFilterRule(); 
        bool getIsValid() const;
};

class FilterGroupEditorState
{
    private:
        bool m_isValid = false;
        size_t m_columnIndex = 0;
        size_t m_filterGroupIndex = 0;
        SCHEDULE_TYPE m_type = SCH_LAST;
        FilterGroup m_filterGroup = FilterGroup();
    public:
        void setup(SCHEDULE_TYPE type, size_t columnIndex, size_t filterGroupIndex, FilterGroup filterGroup);
        SCHEDULE_TYPE getType() const;
        size_t getColumnIndex() const;
        size_t getFilterGroupIndex() const;
        FilterGroup& getFilterGroup(); 
        bool getIsValid() const;
};

class FilterRuleEditorSubGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;
        FilterRuleEditorState m_filterRuleState;
        FilterGroupEditorState& m_filterGroupState;
        bool m_editing = false;
        std::string m_columnName = "";
        unsigned int m_viewedYear = 0;
        unsigned int m_viewedMonth = 0;
        bool m_openNextFrame = false;
        ImRect m_avoidRect;
    public:
        FilterRuleEditorSubGui(const char* ID, const ScheduleCore* scheduleCore, FilterGroupEditorState& filterGroupState);
        
        Event<size_t, size_t, size_t, FilterRuleContainer> addColumnFilterRule;
        Event<size_t, size_t, size_t, size_t, FilterRuleContainer, FilterRuleContainer> editColumnFilterRule;

        void draw(Window& window, Input& input) override;
        // open the editor to edit a pre-existing FilterRule
        void openEdit(SCHEDULE_TYPE type, const std::string& columnName, size_t filterIndex, size_t ruleIndex, const ImRect& avoidRect);
        // open the editor to add a new FilterRule to a Filter
        void openCreate(SCHEDULE_TYPE type, const std::string& columnName, size_t filterIndex, const ImRect& avoidRect);
        // close the FilterRule editor popup if it is open
        void close();

        bool getMadeEdits() const;
};

class FilterEditorSubGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;
        FilterGroupEditorState m_filterGroupState;
        ImRect m_avoidRect;

        std::function<void(size_t)> columnAddedListener = std::function<void(size_t)>([&](size_t column)
        {
            if (column <= m_filterGroupState.getColumnIndex())
            {
                close();
            }
        });
        std::function<void(size_t)> columnRemovedListener = std::function<void(size_t)>([&](size_t column)
        {
            if (column <= m_filterGroupState.getColumnIndex())
            {
                close();
            }
        });
    public:
        FilterEditorSubGui(const char* ID, const ScheduleCore* scheduleCore, ScheduleEvents& scheduleEvents);

        // Events
        Event<size_t, FilterGroup> addColumnFilterGroup;
        Event<size_t, size_t, std::string> setColumnFilterGroupName;
        Event<size_t, size_t, LogicalOperatorEnum> setColumnFilterGroupOperator;
        Event<size_t, size_t> removeColumnFilterGroup;

        Event<size_t, size_t, Filter> addColumnFilter;
        Event<size_t, size_t, size_t, LogicalOperatorEnum> setColumnFilterOperator;
        Event<size_t, size_t, size_t> removeColumnFilter;

        Event<size_t, size_t, size_t, size_t> removeColumnFilterRule;

        void draw(Window& window, Input& input) override;
        void drawRuleEditor();
        // open the editor to edit a pre-existing FilterGroup
        void openGroupEdit(size_t column, size_t filterGroupIndex, const ImRect& avoidRect);
        // quickly create a new FilterGroup for a Column and then run openGroupEdit() to edit it.
        void createGroupAndEdit(size_t column, const ImRect& avoidRect);
        // close the filter editor popup if it is open
        void close();

        bool getMadeEdits() const;
        size_t getColumn() const;
};