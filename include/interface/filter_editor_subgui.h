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
        }
        SCHEDULE_TYPE getType() const;
        std::pair<size_t, size_t> getIndices() const;
        FilterRuleContainer& getFilterRule(); 
        bool getIsValid() const;
};

class FilterEditorSubGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;
        FilterGroupEditorState m_filterGroupState;
        FilterRuleEditorState m_filterRuleState;
        bool m_editing = false;
        size_t m_editorColumn = 0;
        size_t m_editorFilterGroupIndex = 0;
        unsigned int m_viewedYear = 0;
        unsigned int m_viewedMonth = 0;
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
        void open_edit(size_t column, size_t filterGroupIndex, const ImRect& avoidRect);
        // open the editor to add a new FilterGroup to a Column
        void open_create(size_t column, const ImRect& avoidRect);
        // close the filter editor popup if it is open
        void close();

        template <typename T>
        void invokeFilterEditEvent(size_t filterIndex, size_t filterRuleIndex, FilterRule<T> previousValue, FilterRule<T> newValue)
        {
            if (m_editing)
            {
                editColumnFilterRule.invoke(m_editorColumn, m_editorFilterGroupIndex, filterIndex, filterRuleIndex, FilterRuleContainer().fill(previousValue), FilterRuleContainer().fill(newValue));
            }
        };

        bool getMadeEdits() const;
        size_t getColumn() const;
};