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

class EditorFilterState
{
    private:
        std::shared_ptr<FilterRuleBase> m_filter = NULL;
        SCHEDULE_TYPE m_type = SCH_LAST;
    public:
        void setFilter(const std::shared_ptr<FilterRuleBase>& filter);
        template <typename T>
        std::shared_ptr<FilterRule<T>> getFilter()
        {
            if (hasValidFilter() == false) 
            { 
                printf("EditorFilterState::getFilter(): Can't cast invalid filter pointer! Returning dummy shared_ptr\n"); 
                auto errorReturn = std::make_shared<FilterRule<T>>(FilterRule<T>(T()));
                return errorReturn; 
            }
            return std::dynamic_pointer_cast<FilterRule<T>>(getFilterBase());
        }
        std::shared_ptr<FilterRuleBase> getFilterBase();
        void setType(SCHEDULE_TYPE type);
        SCHEDULE_TYPE getType() const;
        // only checks if the filter shared pointer is valid (so not NULL i guess?)
        bool hasValidFilter() const;
};

class FilterEditorSubGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;
        EditorFilterState m_filterState;
        bool m_editing = false;
        size_t m_editorColumn = 0;
        size_t m_editorFilterIndex = 0;
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
        Event<size_t, std::shared_ptr<FilterRuleBase>> addColumnFilter;
        Event<size_t, size_t, std::shared_ptr<FilterRuleBase>, std::shared_ptr<FilterRuleBase>> editColumnFilter;
        Event<size_t, size_t> removeColumnFilter;

        void draw(Window& window, Input& input) override;
        // open the editor to edit a pre-existing FilterRule
        void open_edit(size_t column, size_t filterIndex, const ImRect& avoidRect);
        // open the editor to add a new FilterRule to a Column
        void open_create(size_t column, const ImRect& avoidRect);
        // close the filter editor popup if it is open
        void close();

        template <typename T>
        void invokeFilterEditEvent(FilterRule<T> previousValue, FilterRule<T> newValue)
        {
            if (m_editing)
            {
                editColumnFilter.invoke(m_editorColumn, m_editorFilterIndex, std::make_shared<FilterRule<T>>(previousValue), std::make_shared<FilterRule<T>>(newValue));
            }
        };

        bool getMadeEdits() const;
        size_t getFilterColumn() const;
};