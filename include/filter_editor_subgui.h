#pragma once

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

class FilterEditorSubGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;
        const char* m_dateModeOptions = "is relative to today\0is\0";
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
        GuiEvent<size_t, size_t, std::shared_ptr<FilterBase>> editColumnFilter;
        GuiEvent<size_t, size_t> removeColumnFilter;

        void draw(Window& window, Input& input) override;
        // Update the filter editor before editing a Filter.
        // NOTE: Sets m_madeEdits = false
        // open the editor to edit a pre-existing Filter
        void open_edit(size_t column, size_t filterIndex, const ImRect& avoidRect);
        // open the editor to add a new Filter to a Column
        void open_create(size_t column, const ImRect& avoidRect);

        bool getOpenLastFrame() const;
        bool getOpenThisFrame() const;
        bool getMadeEdits() const;
        size_t getFilterColumn() const;
};