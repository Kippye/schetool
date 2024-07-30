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
        void makeInvalid();
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
        const ScheduleCore& m_scheduleCore;
        FilterRuleEditorState m_filterRuleState;
        FilterGroupEditorState& m_filterGroupState;
        bool m_editing = false;
        std::string m_columnName = "";
        unsigned int m_viewedYear = 1900;
        unsigned int m_viewedMonth = 1;
        bool m_popupOpen = false;
        bool m_openNextFrame = false;
        ImRect m_avoidRect;

        // Types of edits listened to by this gui's edit undo / redo listeners.
        // NOTE: Their corresponding classes should ALL inherit from FilterEditBase
        std::set<ScheduleEditType> m_filterEditTypes =
        {
            ScheduleEditType::FilterGroupAddOrRemove,
            ScheduleEditType::FilterGroupChange,
            ScheduleEditType::FilterAddOrRemove,
            ScheduleEditType::FilterChange,
            ScheduleEditType::FilterRuleAddOrRemove,
            ScheduleEditType::FilterRuleChange,
        };

        std::function<void(std::shared_ptr<const ScheduleEdit>)> editUndoListener = [&](std::shared_ptr<const ScheduleEdit> edit)
        {
            // not even editing so why update?
            if (m_filterGroupState.getIsValid() == false) { return; }
            // not a filter edit, don't care
            if (m_filterEditTypes.contains(edit->getType()) == false) { return; }
            // not editing this column and filter group -> don't need to update gui
            auto filterEditBase = std::dynamic_pointer_cast<const FilterEditBase>(edit);
            if (filterEditBase->getColumnIndex() != m_filterGroupState.getColumnIndex() || filterEditBase->getFilterGroupIndex() != m_filterGroupState.getFilterGroupIndex()) { return; }

            switch(edit->getType())
            {
                case(ScheduleEditType::FilterGroupAddOrRemove):
                {
                    // ADD + UNDO = REMOVE THIS GROUP
                    // Close all the filter editors
                    if (std::dynamic_pointer_cast<const FilterGroupAddOrRemoveEdit>(edit)->getIsRemove() == false)
                    {
                        m_filterRuleState.makeInvalid(); // invalidate the state (causes popup to close)
                        m_filterGroupState.makeInvalid(); // should cause filter group editor to close as well
                    }
                    break;
                }
                case(ScheduleEditType::FilterGroupChange):
                {
                    auto filterGroupChange = std::dynamic_pointer_cast<const FilterGroupChangeEdit>(edit);
                    m_filterGroupState.getFilterGroup().setOperator(filterGroupChange->getPrevOperator());
                    m_filterGroupState.getFilterGroup().setName(filterGroupChange->getPrevName());
                    break;
                }
                case(ScheduleEditType::FilterAddOrRemove):
                {
                    auto filterAddOrRemove = std::dynamic_pointer_cast<const FilterAddOrRemoveEdit>(edit);

                    if (filterAddOrRemove->getIsRemove()) // REMOVE + UNDO = ADD
                    {
                        m_filterGroupState.getFilterGroup().addFilter(filterAddOrRemove->getFilterIndex(), filterAddOrRemove->getFilter());
                    }
                    else // ADD + UNDO = REMOVE
                    {
                        m_filterGroupState.getFilterGroup().removeFilter(filterAddOrRemove->getFilterIndex());
                    }
                    break;
                }
                case(ScheduleEditType::FilterChange):
                {
                    auto filterChange = std::dynamic_pointer_cast<const FilterChangeEdit>(edit);
                    m_filterGroupState.getFilterGroup().getFilter(filterChange->getFilterIndex()).setOperator(filterChange->getPrevOperator());
                    break;
                }
                case(ScheduleEditType::FilterRuleAddOrRemove):
                {
                    auto ruleAddOrRemoveBase = std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEditBase>(edit);

                    // TODO: Handle removing of the FilterRule being edited
                    // I think i also need to handle the index offset when one is added before the one being edited? Crazy stuff.
                    // Maybe just close the editor if this happens lol. Give up.

                    if (ruleAddOrRemoveBase->getIsRemove()) // REMOVE + UNDO = ADD
                    {
                        switch(m_filterGroupState.getType())
                        {
                            case(SCH_BOOL):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<bool>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_NUMBER):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<int>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_DECIMAL):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<double>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_TEXT):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<std::string>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_SELECT):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<SelectContainer>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_WEEKDAY):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<WeekdayContainer>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_TIME):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<TimeContainer>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_DATE):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<DateContainer>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_LAST):
                                break;
                        }
                    }
                    else // ADD + UNDO = REMOVE
                    {
                        m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).removeRule(ruleAddOrRemoveBase->getFilterRuleIndex());
                    }
                    break;
                }
                case(ScheduleEditType::FilterRuleChange):
                {
                    auto ruleChangeBase = std::dynamic_pointer_cast<const FilterRuleChangeEditBase>(edit);

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
        std::function<void(std::shared_ptr<const ScheduleEdit>)> editRedoListener = [&](std::shared_ptr<const ScheduleEdit> edit)
        {
            // not even open so why update?
            if (m_filterGroupState.getIsValid() == false) { return; }
            // not a filter edit, don't care
            if (m_filterEditTypes.contains(edit->getType()) == false) { return; }
            // not editing this column and filter group -> don't need to update gui
            auto filterEditBase = std::dynamic_pointer_cast<const FilterEditBase>(edit);
            if (filterEditBase->getColumnIndex() != m_filterGroupState.getColumnIndex() || filterEditBase->getFilterGroupIndex() != m_filterGroupState.getFilterGroupIndex()) { return; }

            switch(edit->getType())
            {
                case(ScheduleEditType::FilterGroupAddOrRemove):
                {
                    // REMOVE + REDO = REMOVE THIS GROUP
                    // Close all the filter editors
                    if (std::dynamic_pointer_cast<const FilterGroupAddOrRemoveEdit>(edit)->getIsRemove())
                    {
                        m_filterRuleState.makeInvalid(); // invalidate the state (causes popup to close)
                        m_filterGroupState.makeInvalid(); // should cause filter group editor to close as well
                    }
                    break;
                }
                case(ScheduleEditType::FilterGroupChange):
                {
                    auto filterGroupChange = std::dynamic_pointer_cast<const FilterGroupChangeEdit>(edit);
                    m_filterGroupState.getFilterGroup().setOperator(filterGroupChange->getNewOperator());
                    m_filterGroupState.getFilterGroup().setName(filterGroupChange->getNewName());
                    break;
                }
                case(ScheduleEditType::FilterAddOrRemove):
                {
                    auto filterAddOrRemove = std::dynamic_pointer_cast<const FilterAddOrRemoveEdit>(edit);

                    if (filterAddOrRemove->getIsRemove() == false) // ADD + REDO = ADD
                    {
                        m_filterGroupState.getFilterGroup().addFilter(filterAddOrRemove->getFilterIndex(), filterAddOrRemove->getFilter());
                    }
                    else // REMOVE + REDO = REMOVE
                    {
                        m_filterGroupState.getFilterGroup().removeFilter(filterAddOrRemove->getFilterIndex());
                    }
                    break;
                }
                case(ScheduleEditType::FilterChange):
                {
                    auto filterChange = std::dynamic_pointer_cast<const FilterChangeEdit>(edit);
                    m_filterGroupState.getFilterGroup().getFilter(filterChange->getFilterIndex()).setOperator(filterChange->getNewOperator());
                    break;
                }
                case(ScheduleEditType::FilterRuleAddOrRemove):
                {
                    auto ruleAddOrRemoveBase = std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEditBase>(edit);

                    // TODO: Handle removing of the FilterRule being edited
                    // I think i also need to handle the index offset when one is added before the one being edited? Crazy stuff.
                    // Maybe just close the editor if this happens lol. Give up.

                    // ADD + REDO = ADD
                    if (ruleAddOrRemoveBase->getIsRemove() == false)
                    {
                        switch (m_filterGroupState.getType())
                        {
                            case(SCH_BOOL):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<bool>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_NUMBER):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<int>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_DECIMAL):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<double>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_TEXT):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<std::string>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_SELECT):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<SelectContainer>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_WEEKDAY):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<WeekdayContainer>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_TIME):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<TimeContainer>>(ruleAddOrRemoveBase)->getRule());
                                break;
                            case(SCH_DATE):
                                m_filterGroupState.getFilterGroup().getFilter(ruleAddOrRemoveBase->getFilterIndex()).addRule(ruleAddOrRemoveBase->getFilterRuleIndex(), std::dynamic_pointer_cast<const FilterRuleAddOrRemoveEdit<DateContainer>>(ruleAddOrRemoveBase)->getRule());
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
                    break;
                }
                case(ScheduleEditType::FilterRuleChange):
                {
                    auto ruleChangeBase = std::dynamic_pointer_cast<const FilterRuleChangeEditBase>(edit);

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
                default: break;
            }
        };
    public:
        FilterRuleEditorSubGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents, FilterGroupEditorState& filterGroupState);
        
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
        const ScheduleCore& m_scheduleCore;
        FilterGroupEditorState m_filterGroupState;
        bool m_popupOpen = false;
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
        FilterEditorSubGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents);

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