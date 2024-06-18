#pragma once

#include <map>
#include "gui.h"
#include "filter.h"
#include "select_container.h"
#include "time_container.h"
#include "date_container.h"
#include "element_base.h"
#include "schedule_core.h"

enum class DateMode : int
{
    Relative,
    Absolute
};

class EditorFilterState
{
    private:
        std::shared_ptr<FilterBase> m_filter = NULL;
        SCHEDULE_TYPE m_type = SCH_LAST;
    public:
        void setFilter(const std::shared_ptr<FilterBase>& filter);
        template <typename T>
        std::shared_ptr<Filter<T>> getFilter()
        {
            if (hasValidFilter() == false) 
            { 
                printf("EditorFilterState::getFilter(): Can't cast invalid filter pointer! Returning dummy shared_ptr\n"); 
                auto errorReturn = std::make_shared<Filter<T>>(Filter<T>(T()));
                return errorReturn; 
            }
            return std::dynamic_pointer_cast<Filter<T>>(getFilterBase());
        }
        std::shared_ptr<FilterBase> getFilterBase();
        void setType(SCHEDULE_TYPE type);
        SCHEDULE_TYPE getType() const;
        // only checks if the filter shared pointer is valid (so not NULL i guess?)
        bool hasValidFilter() const;
};

struct ComparisonOptions
{
    public:
        const char* string;
        const size_t count;
        ComparisonOptions(const char* string, size_t count) : string(string), count(count) {}
};

class TypeComparisonOptions
{
    private:
        std::map<SCHEDULE_TYPE, ComparisonOptions> m_comparisonOptions =
        {
            { SCH_BOOL,     ComparisonOptions("is\0", 1) },
            { SCH_NUMBER,   ComparisonOptions("is\0", 1) },
            { SCH_DECIMAL,  ComparisonOptions("is\0", 1) },
            { SCH_TEXT,     ComparisonOptions("is\0", 1) },
            { SCH_SELECT,   ComparisonOptions("is\0", 1) },
            { SCH_TIME,     ComparisonOptions("is\0", 1) },
            { SCH_DATE,     ComparisonOptions("is relative to today\0is\0", 2) },
        };
        std::map<SCHEDULE_TYPE, int> m_selectedOptions =
        {
            { SCH_BOOL,     0 },
            { SCH_NUMBER,   0 },
            { SCH_DECIMAL,  0 },
            { SCH_TEXT,     0 },
            { SCH_SELECT,   0 },
            { SCH_TIME,     0 },
            { SCH_DATE,     0 },
        };
    public:
        ComparisonOptions getOptions(SCHEDULE_TYPE type);
        int getOptionSelection(SCHEDULE_TYPE type);
        void setOptionSelection(SCHEDULE_TYPE type, int selection);
};

class FilterEditorSubGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;
        TypeComparisonOptions m_typeComparisonOptions;
        DateMode m_selectedDateMode = DateMode::Relative;
        EditorFilterState m_filterState;
        bool m_openLastFrame = false;
        bool m_openThisFrame = false;
        bool m_madeEdits = false; 
        bool m_editing = false;
        size_t m_editorColumn = 0;
        size_t m_editorFilterIndex = 0;
        unsigned int m_viewedYear = 0;
        unsigned int m_viewedMonth = 0;
        ImRect m_avoidRect;
    public:
        FilterEditorSubGui(const char* ID, const ScheduleCore* scheduleCore);

        // Events
        GuiEvent<size_t, std::shared_ptr<FilterBase>> addColumnFilter;
        GuiEvent<size_t, size_t, std::shared_ptr<FilterBase>, std::shared_ptr<FilterBase>> editColumnFilter;
        GuiEvent<size_t, size_t> removeColumnFilter;

        void draw(Window& window, Input& input) override;
        // open the editor to edit a pre-existing Filter
        void open_edit(size_t column, size_t filterIndex, const ImRect& avoidRect);
        // open the editor to add a new Filter to a Column
        void open_create(size_t column, const ImRect& avoidRect);

        template <typename T>
        void invokeFilterEditEvent(Filter<T> previousValue, Filter<T> newValue)
        {
            if (m_editing)
            {
                editColumnFilter.invoke(m_editorColumn, m_editorFilterIndex, std::make_shared<Filter<T>>(previousValue), std::make_shared<Filter<T>>(newValue));
            }
        };

        bool getOpenLastFrame() const;
        bool getOpenThisFrame() const;
        bool getMadeEdits() const;
        size_t getFilterColumn() const;
};