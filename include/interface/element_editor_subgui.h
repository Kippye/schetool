#pragma once

#include "gui.h"
#include "single_select_container.h"
#include "select_container.h"
#include "weekday_container.h"
#include "time_container.h"
#include "date_container.h"
#include "element_base.h"
#include "schedule_core.h"

class ElementEditorSubGui : public Gui
{
    private:
        const ScheduleCore& m_scheduleCore;
        SCHEDULE_TYPE m_editedType;
        bool m_openLastFrame = false;
        bool m_openThisFrame = false;
        bool m_madeEdits = false;
        // Drag and drop option reordering
        std::string m_draggedOptionID = "";
        bool m_hasOptionBeenDragged = false;
        
        int m_editorColumn = -1;
        int m_editorRow = -1;
        unsigned int m_viewedYear = 0;
        unsigned int m_viewedMonth = 0;
        std::string m_editorText;
        TimeContainer m_editorTime;
        DateContainer m_editorDate;
        SingleSelectContainer m_editorSingleSelect;
        SelectContainer m_editorSelect;
        WeekdayContainer m_editorWeekday;

        // Used to automatically focus the input textbox one frame after it was made visible. Set to false after doing so!
        bool m_giveSelectOptionNameInputFocus = false;
        bool m_editingSelectOptionName = false;
        size_t m_editedOptionIndex = 0;

        // Select option color chooser: index of the select option being modified
        size_t m_colorChooserOptionIndex = 0;
        
        ImRect m_avoidRect;
        ImVec2 m_textInputBoxSize = ImVec2(0, 0);
    public:
        ElementEditorSubGui(const char* ID, const ScheduleCore& scheduleCore);

        // Events
        // modifyColumnSelectOptions
        Event<size_t, SelectOptionsModification> modifyColumnSelectOptions;

        void draw(Window& window, Input& input, GuiTextures& guiTextures) override;
        // Update the element editor before editing a new Element.
        // NOTE: Sets m_madeEdits = false
        void open(size_t column, size_t row, SCHEDULE_TYPE type, const ImRect& avoidRect);
        void setTextInputBoxSize(ImVec2 size);
        void setEditorValue(const std::string& value)
        {
            m_editorText = value;
        }
        void setEditorValue(const TimeContainer& value)
        {
            m_editorTime = value;
        }
        // NOTE: Also sets m_viewedMonth and m_viewedYear to the DateContainer's month and year
        void setEditorValue(const DateContainer& value)
        {
            m_editorDate = value;
            m_viewedMonth = m_editorDate.getIsEmpty() ? DateContainer::getCurrentSystemDate().getTime().getMonth() : m_editorDate.getTime().getMonth();
            m_viewedYear = m_editorDate.getIsEmpty() ? DateContainer::getCurrentSystemDate().getTime().getYear() : m_editorDate.getTime().getYear();
        }
        void setEditorValue(const SingleSelectContainer& value)
        {
            m_editorSingleSelect = value;
        }
        void setEditorValue(const SelectContainer& value)
        {
            m_editorSelect = value;
        }
        void setEditorValue(const WeekdayContainer& value)
        {
            m_editorWeekday = value;
        }
        std::string getEditorValue(const std::string& _typeValueUnused) const
        {
            return m_editorText;
        }
        const TimeContainer& getEditorValue(const TimeContainer& _typeValueUnused) const
        {
            return m_editorTime;
        }
        const DateContainer& getEditorValue(const DateContainer& _typeValueUnused) const
        {
            return m_editorDate;
        }
        const SingleSelectContainer& getEditorValue(const SingleSelectContainer& _typeValueUnused) const
        {
            return m_editorSingleSelect;
        }
        const SelectContainer& getEditorValue(const SelectContainer& _typeValueUnused) const
        {
            return m_editorSelect;
        }
        const WeekdayContainer& getEditorValue(const WeekdayContainer& _typeValueUnused) const
        {
            return m_editorWeekday;
        }
        bool getOpenLastFrame() const;
        bool getOpenThisFrame() const;
        bool getMadeEdits() const;
        std::pair<size_t, size_t> getCoordinates() const;
};