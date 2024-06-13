#pragma once

#include "gui.h"
#include "select_container.h"
#include "time_container.h"
#include "date_container.h"
#include "element_base.h"
#include "schedule_core.h"

class ElementEditorSubGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;
        SCHEDULE_TYPE m_editedType;
        bool m_openLastFrame = false;
        bool m_openThisFrame = false;
        bool m_madeEdits = false; 
        int m_editorColumn = -1;
        int m_editorRow = -1;
        unsigned int m_viewedYear = 0;
        unsigned int m_viewedMonth = 0;
        TimeContainer m_editorTime;
        DateContainer m_editorDate;
        SelectContainer m_editorSelect;
        ImRect m_avoidRect;
    public:
        ElementEditorSubGui(const char* ID, const ScheduleCore* scheduleCore);

        // Events
        // modifyColumnSelectOptions
        GuiEvent<size_t, SelectOptionsModification> modifyColumnSelectOptions;

        void draw(Window& window, Input& input) override;
        // Update the element editor before editing a new Element.
        // NOTE: Sets m_madeEdits = false
        void open(size_t column, size_t row, SCHEDULE_TYPE type, const ImRect& avoidRect);
        void setEditorValue(const TimeContainer& value)
        {
            m_editorTime = value;
        }
        // NOTE: Also sets m_viewedMonth and m_viewedYear to the DateContainer's month and year
        void setEditorValue(const DateContainer& value)
        {
            m_editorDate = value;
            m_viewedMonth = m_editorDate.getTime().tm_mon;
            m_viewedYear = m_editorDate.getTime().tm_year;
        }
        void setEditorValue(const SelectContainer& value)
        {
            m_editorSelect = value;
        }
        const TimeContainer& getEditorValue(const TimeContainer& _typeValueUnused) const
        {
            return m_editorTime;
        }
        const DateContainer& getEditorValue(const DateContainer& _typeValueUnused) const
        {
            return m_editorDate;
        }
        const SelectContainer& getEditorValue(const SelectContainer& _typeValueUnused) const
        {
            return m_editorSelect;
        }
        bool getOpenLastFrame() const;
        bool getOpenThisFrame() const;
        bool getMadeEdits() const;
        std::pair<size_t, size_t> getCoordinates() const;
        static int filterNumbers(ImGuiInputTextCallbackData* data);
};