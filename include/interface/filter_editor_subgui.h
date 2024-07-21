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
#include "schedule_edit.h"

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
            m_filterRule = FilterRuleContainer(filterRule);
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
        void makeInvalid();
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

        std::function<void(const ScheduleEdit*)> editUndoListener = [&](const ScheduleEdit* edit)
        {
            // not even editing so why update?
            if (m_filterGroupState.getIsValid() == false) { return; }

            switch(edit->getType())
            {
                case(ScheduleEditType::FilterRuleAddOrRemove):
                {
                    auto ruleAddOrRemoveBase = (const FilterRuleAddOrRemoveEditBase*)edit;

                    // TODO: Handle removing of the FilterRule being edited
                    // I think i also need to handle the index offset when one is added before the one being edited? Crazy stuff.
                    // Maybe just close the editor if this happens lol. Give up.

                    // the rule was added or remove from the column and filtergroup currently being edited
                    if (ruleAddOrRemoveBase->getColumnIndex() == m_filterGroupState.getColumnIndex() && ruleAddOrRemoveBase->getFilterGroupIndex() == m_filterGroupState.getFilterGroupIndex())
                    {
                        // REMOVE + UNDO = ADD
                        if (ruleAddOrRemoveBase->getIsRemove())
                        {
                            switch(m_filterGroupState.getType())
                            {
                                case(SCH_BOOL):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<bool>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_NUMBER):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<int>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_DECIMAL):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<double>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_TEXT):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<std::string>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_SELECT):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<SelectContainer>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_WEEKDAY):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<WeekdayContainer>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_TIME):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<TimeContainer>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_DATE):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<DateContainer>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_LAST):
                                    break;
                            }
                        }
                        // ADD + UNDO = REMOVE
                        else
                        {
                            m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).removeRule(ruleAddOrRemoveBase->getFilterRuleIndex());
                        }
                    }
                    break;
                }
                case(ScheduleEditType::FilterRuleChange):
                {
                    auto ruleChangeBase = (const FilterRuleChangeEditBase*)edit;

                    switch(m_filterGroupState.getType())
                    {
                        case(SCH_BOOL):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getPrevRule().getAsType<bool>());
                            break;
                        case(SCH_NUMBER):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getPrevRule().getAsType<int>());
                            break;
                        case(SCH_DECIMAL):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getPrevRule().getAsType<double>());
                            break;
                        case(SCH_TEXT):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getPrevRule().getAsType<std::string>());
                            break;
                        case(SCH_SELECT):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getPrevRule().getAsType<SelectContainer>());
                            break;
                        case(SCH_WEEKDAY):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getPrevRule().getAsType<WeekdayContainer>());
                            break;
                        case(SCH_TIME):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getPrevRule().getAsType<TimeContainer>());
                            break;
                        case(SCH_DATE):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getPrevRule().getAsType<DateContainer>());
                            break;
                        case(SCH_LAST):
                            break;
                    }
                    break;
                }
                default: break;
            }
        };
        std::function<void(const ScheduleEdit*)> editRedoListener = [&](const ScheduleEdit* edit)
        {
            // not even open so why update?
            if (m_filterGroupState.getIsValid() == false) { return; }

            switch(edit->getType())
            {
                case(ScheduleEditType::FilterRuleAddOrRemove):
                {
                    auto ruleAddOrRemoveBase = (const FilterRuleAddOrRemoveEditBase*)edit;

                    // TODO: Handle removing of the FilterRule being edited
                    // I think i also need to handle the index offset when one is added before the one being edited? Crazy stuff.
                    // Maybe just close the editor if this happens lol. Give up.

                    // the rule was added or remove from the column and filtergroup currently being edited
                    if (ruleAddOrRemoveBase->getColumnIndex() == m_filterGroupState.getColumnIndex() && ruleAddOrRemoveBase->getFilterGroupIndex() == m_filterGroupState.getFilterGroupIndex())
                    {
                        // ADD + REDO = ADD
                        if (ruleAddOrRemoveBase->getIsRemove() == false)
                        {
                            switch (m_filterGroupState.getType())
                            {
                                case(SCH_BOOL):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<bool>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_NUMBER):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<int>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_DECIMAL):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<double>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_TEXT):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<std::string>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_SELECT):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<SelectContainer>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_WEEKDAY):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<WeekdayContainer>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_TIME):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<TimeContainer>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_DATE):
                                    m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(((FilterRuleAddOrRemoveEdit<DateContainer>*)ruleAddOrRemoveBase)->getRule());
                                    break;
                                case(SCH_LAST):
                                    break;
                            }
                        }
                        // REMOVE + REDO = REMOVE
                        else
                        {
                            m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).removeRule(ruleAddOrRemoveBase->getFilterRuleIndex());
                        }
                    }
                    break;
                }
                case(ScheduleEditType::FilterRuleChange):
                {
                    auto ruleChangeBase = (const FilterRuleChangeEditBase*)edit;

                    switch(m_filterGroupState.getType())
                    {
                        case(SCH_BOOL):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getNewRule().getAsType<bool>());
                            break;
                        case(SCH_NUMBER):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getNewRule().getAsType<int>());
                            break;
                        case(SCH_DECIMAL):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getNewRule().getAsType<double>());
                            break;
                        case(SCH_TEXT):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getNewRule().getAsType<std::string>());
                            break;
                        case(SCH_SELECT):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getNewRule().getAsType<SelectContainer>());
                            break;
                        case(SCH_WEEKDAY):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getNewRule().getAsType<WeekdayContainer>());
                            break;
                        case(SCH_TIME):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getNewRule().getAsType<TimeContainer>());
                            break;
                        case(SCH_DATE):
                            m_filterGroupState.getFilterGroup().getFilter(ruleChangeBase->getFilterIndex()).replaceRule(ruleChangeBase->getFilterRuleIndex(), ruleChangeBase->getNewRule().getAsType<DateContainer>());
                            break;
                        case(SCH_LAST):
                            break;
                    }
                    break;
                }
            }
        };
    public:
        FilterRuleEditorSubGui(const char* ID, const ScheduleCore* scheduleCore, ScheduleEvents& scheduleEvents, FilterGroupEditorState& filterGroupState);
        
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

        std::function<void(size_t)> columnAddedListener = [&](size_t column)
        {
            if (column <= m_filterGroupState.getColumnIndex())
            {
                close();
            }
        };
        std::function<void(size_t)> columnRemovedListener = [&](size_t column)
        {
            if (column <= m_filterGroupState.getColumnIndex())
            {
                close();
            }
        };
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