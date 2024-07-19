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
        SCHEDULE_TYPE m_type = SCH_LAST;
        FilterGroup m_filterGroup = FilterGroup();
    public:
        void setup(SCHEDULE_TYPE type, FilterGroup filterGroup);
        SCHEDULE_TYPE getType() const;
        FilterGroup& getFilterGroup(); 
        bool getIsValid() const;
};

class FilterRuleEditorSubGui : public Gui
{
    private:
        FilterRuleEditorState m_filterRuleState;
        FilterGroupEditorState& m_filterGroupState;
        bool m_editing = false;
        std::string m_columnName = "";
        unsigned int m_viewedYear = 0;
        unsigned int m_viewedMonth = 0;
        bool m_openNextFrame = false;
        ImRect m_avoidRect;
        // PRIVATE Events only for FilterEditorSubGui
        Event<size_t, size_t, FilterRuleContainer> filterRuleAdded;
        Event<size_t, size_t, size_t, FilterRuleContainer, FilterRuleContainer> filterRuleEdited;
        Event<size_t, size_t, size_t> filterRuleRemoved;
    public:
        FilterRuleEditorSubGui(const char* ID, FilterGroupEditorState& filterGroupState);

        void draw(Window& window, Input& input) override;
        // open the editor to edit a pre-existing FilterRule
        void openEdit(SCHEDULE_TYPE type, const std::string& columnName, size_t filterIndex, size_t ruleIndex, const ImRect& avoidRect);
        // open the editor to add a new FilterRule to a Filter
        void openCreate(SCHEDULE_TYPE type, const std::string& columnName, size_t filterIndex, const ImRect& avoidRect);
        // close the FilterRule editor popup if it is open
        void close();

        bool getMadeEdits() const;

        // Invokes the editColumnFilterRule event.
        // template <typename T>
        // void invokeFilterRuleEditEvent(size_t filterIndex, size_t filterRuleIndex, FilterRule<T> previousValue, FilterRule<T> newValue)
        // {
        //     if (m_editing)
        //     {
        //         editColumnFilterRule.invoke(m_editorColumn, m_editorFilterGroupIndex, filterIndex, filterRuleIndex, FilterRuleContainer().fill(previousValue), FilterRuleContainer().fill(newValue));
        //     }
        // };
};

class FilterEditorSubGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;
        FilterGroupEditorState m_filterGroupState;
        bool m_editing = false;
        size_t m_editorColumn = 0;
        size_t m_editorFilterGroupIndex = 0;
        ImRect m_avoidRect;

        std::function<void(size_t)> columnAddedListener = std::function<void(size_t)>([&](size_t column)
        {
            if (column <= m_editorColumn)
            {
                close();
            }
        });
        std::function<void(size_t)> columnRemovedListener = std::function<void(size_t)>([&](size_t column)
        {
            if (column <= m_editorColumn)
            {
                close();
            }
        });
    public:
        FilterEditorSubGui(const char* ID, const ScheduleCore* scheduleCore, ScheduleEvents& scheduleEvents);

        // Events
        Event<size_t, FilterGroup> addColumnFilterGroup;
        Event<size_t, size_t> removeColumnFilterGroup;

        Event<size_t, size_t, Filter> addColumnFilter;
        Event<size_t, size_t, size_t> removeColumnFilter;

        Event<size_t, size_t, size_t, FilterRuleContainer> addColumnFilterRule;
        Event<size_t, size_t, size_t, size_t, FilterRuleContainer, FilterRuleContainer> editColumnFilterRule;
        Event<size_t, size_t, size_t, size_t> removeColumnFilterRule;

        void draw(Window& window, Input& input) override;
        void drawRuleEditor();
        // open the editor to edit a pre-existing FilterGroup
        void openGroupEdit(size_t column, size_t filterGroupIndex, const ImRect& avoidRect);
        // open the editor to add a new FilterGroup to a Column
        void openGroupCreate(size_t column, const ImRect& avoidRect);
        // close the filter editor popup if it is open
        void close();

        bool getMadeEdits() const;
        size_t getColumn() const;
};