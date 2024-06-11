#pragma once
#include <imgui.h>
#include <gui.h>
#include <window.h>
#include <input.h>
#include <element_base.h>
#include <select_container.h>
#include <select_options.h>
#include <schedule_core.h>
#include <schedule_column.h>

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

class EditorFilterState
{
    private:
        std::shared_ptr<FilterBase> m_filter = NULL;
        SCHEDULE_TYPE m_type = SCH_LAST;
    public:
        void setFilter(const std::shared_ptr<FilterBase>& filter);
        std::shared_ptr<FilterBase>& getFilter();
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
        bool m_openLastFrame = false;
        bool m_openThisFrame = false;
        bool m_madeEdits = false; 
        int m_editorColumn = -1;
        size_t m_editorFilter = 0;
        ImRect m_avoidRect;
    public:
        FilterEditorSubGui(const char* ID, const ScheduleCore* scheduleCore);

        // Events
        GuiEvent<size_t, std::shared_ptr<FilterBase>> addColumnFilter;
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

class ScheduleGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;

        ImVec4 m_dayColours[7] =
        {
            ImVec4(0.0f / 255.0f, 62.0f / 255.0f, 186.0f / 255.0f, 1),
            ImVec4(198.0f / 255.0f, 138.0f / 255.0f, 0.0f / 255.0f, 1),
            ImVec4(0.0f / 255.0f, 160.0f / 255.0f, 16.0f / 255.0f, 1),
            ImVec4(86.0f / 255.0f, 47.0f / 255.0f, 0.0f / 255.0f, 1),
            ImVec4(181.0f / 255.0f, 43.0f / 255.0f, 43.0f / 255.0f, 1),
            ImVec4(94.0f / 255.0f, 60.0f / 255.0f, 188.0f / 255.0f, 1),
            ImVec4(216.0f / 255.0f, 188.0f / 255.0f, 47.0f / 255.0f, 1),
        };
        void displayColumnContextPopup(unsigned int column, ImGuiTableFlags tableFlags);
    public:
        ScheduleGui(const char* ID) : Gui(ID) { }

        // Events
        // setElementValue(column, row, value)
        GuiEvent<size_t, size_t, bool>                      setElementValueBool;
        GuiEvent<size_t, size_t, int>                       setElementValueNumber;
        GuiEvent<size_t, size_t, double>                    setElementValueDecimal;
        GuiEvent<size_t, size_t, std::string>        setElementValueText;
        GuiEvent<size_t, size_t, SelectContainer>    setElementValueSelect;
        GuiEvent<size_t, size_t, TimeContainer>      setElementValueTime;
        GuiEvent<size_t, size_t, DateContainer>      setElementValueDate;
        // column add / remove
        GuiEvent<size_t> removeColumn;
        GuiEvent<size_t> addDefaultColumn;
        // column modification
        GuiEvent<size_t, SCHEDULE_TYPE>         setColumnType;
        GuiEvent<size_t, COLUMN_SORT>           setColumnSort;
        GuiEvent<size_t, std::string>    setColumnName;
        // row modification
        GuiEvent<size_t> addRow;
        GuiEvent<size_t> removeRow;

        // ScheduleGui(const char* ID, Schedule*);
        void setScheduleCore(const ScheduleCore& scheduleCore);
        void draw(Window& window, Input& input) override;
};