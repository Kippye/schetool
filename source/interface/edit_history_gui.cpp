#include <cstdio>
#include <edit_history_gui.h>
#include <schedule_edit.h>
#include <schedule_constants.h>

void EditHistoryGui::draw(Window& window, Input& input)
{
    if (ImGui::Begin("Edit History", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        auto editHistory = m_scheduleEditHistory->getEditHistory();

        for (size_t i = 0; i < editHistory.size(); i++)
        {
            std::string s;
            s.reserve(256);
            char* buf = s.data();

            switch (editHistory[i]->getType())
            {
                case (ScheduleEditType::ElementChange):
                {
                    auto elementEdit = std::dynamic_pointer_cast<ElementEditBase>(editHistory[i]);
                    sprintf(buf, "Element of type %s at (%zu; %zu)##%zu", 
                        schedule_consts::scheduleTypeNames.at(elementEdit->getElementType()), 
                        elementEdit->getPosition().first, 
                        elementEdit->getPosition().second,
                        i);
                    break;
                }
                case (ScheduleEditType::RowAddOrRemove):
                {
                    auto rowEdit = std::dynamic_pointer_cast<RowEdit>(editHistory[i]);
                    sprintf(buf, "%c Row at %zu##%zu", 
                        rowEdit->getIsRemove() ? '-' : '+', 
                        rowEdit->getRow(),
                        i);
                    break;
                }
                case (ScheduleEditType::ColumnAddOrRemove):
                {
                    auto columnEdit = std::dynamic_pointer_cast<ColumnEdit>(editHistory[i]);
                    sprintf(buf, "%c Column %s of type %s at %zu##%zu", 
                        columnEdit->getIsRemove() ? '-' : '+',
                        columnEdit->getColumnData().name.c_str(), 
                        schedule_consts::scheduleTypeNames.at(columnEdit->getColumnData().type),
                        columnEdit->getColumn(),
                        i);
                    break;
                }
                case (ScheduleEditType::ColumnPropertyChange):
                {
                    auto columnPropertyEdit = std::dynamic_pointer_cast<ColumnPropertyEdit>(editHistory[i]);
                    COLUMN_PROPERTY editedProperty = columnPropertyEdit->getEditedProperty();
                    sprintf(buf, "Column %s property %s at %zu##%zu", 
                        columnPropertyEdit->getColumnName().c_str(),
                        editedProperty == COLUMN_PROPERTY_NAME ? "Name" 
                        : (editedProperty == COLUMN_PROPERTY_TYPE ? "Type" 
                        : (editedProperty == COLUMN_PROPERTY_SELECT_OPTIONS ? "Select options" 
                        : (editedProperty == COLUMN_PROPERTY_SORT ? "Sort" : "Reset option"))),
                        columnPropertyEdit->getColumn(),
                        i);
                    break;
                }
                case (ScheduleEditType::ColumnReset):
                {
                    auto columnResetEdit = std::dynamic_pointer_cast<ColumnResetEdit>(editHistory[i]);
                    sprintf(buf, "Reset Column %s of type %s at %zu##%zu", 
                        columnResetEdit->getColumnData().name.c_str(), 
                        schedule_consts::scheduleTypeNames.at(columnResetEdit->getColumnData().type),
                        columnResetEdit->getColumn(),
                        i);
                    break;
                }
                case (ScheduleEditType::FilterGroupAddOrRemove):
                {
                    auto filterGroupEdit = std::dynamic_pointer_cast<FilterGroupAddOrRemoveEdit>(editHistory[i]);
                    sprintf(buf, "%c Filter group #%zu of Column %zu##%zu", 
                        filterGroupEdit->getIsRemove() ? '-' : '+', 
                        filterGroupEdit->getFilterGroupIndex(),
                        filterGroupEdit->getColumnIndex(),
                        i);
                    break;
                }
                case (ScheduleEditType::FilterGroupChange):
                {
                    auto filterGroupChangeEdit = std::dynamic_pointer_cast<FilterGroupChangeEdit>(editHistory[i]);
                    bool operatorChanged = filterGroupChangeEdit->getPrevOperator() != filterGroupChangeEdit->getNewOperator();
                    sprintf(buf, "Edit filter group #%zu of Column %zu %s (%s -> %s) ##%zu", 
                        filterGroupChangeEdit->getFilterGroupIndex(),
                        filterGroupChangeEdit->getColumnIndex(),
                        operatorChanged ? "operator" : "name",
                        operatorChanged ? schedule_consts::logicalOperatorStrings.at(filterGroupChangeEdit->getPrevOperator()) : filterGroupChangeEdit->getPrevName().c_str(),
                        operatorChanged ? schedule_consts::logicalOperatorStrings.at(filterGroupChangeEdit->getNewOperator()) : filterGroupChangeEdit->getNewName().c_str(),
                        i);
                    break;
                }
                case (ScheduleEditType::FilterAddOrRemove):
                {
                    auto filterEdit = std::dynamic_pointer_cast<FilterAddOrRemoveEdit>(editHistory[i]);
                    sprintf(buf, "%c Filter #%zu in group #%zu of Column %zu##%zu", 
                        filterEdit->getIsRemove() ? '-' : '+', 
                        filterEdit->getFilterIndex(),
                        filterEdit->getFilterGroupIndex(),
                        filterEdit->getColumnIndex(),
                        i);
                    break;
                }
                case (ScheduleEditType::FilterChange):
                {
                    auto filterChangeEdit = std::dynamic_pointer_cast<FilterChangeEdit>(editHistory[i]);
                    sprintf(buf, "Edit filter #%zu in group #%zu of Column %zu operator (%s -> %s)##%zu", 
                        filterChangeEdit->getFilterIndex(),
                        filterChangeEdit->getFilterGroupIndex(),
                        filterChangeEdit->getColumnIndex(),
                        schedule_consts::logicalOperatorStrings.at(filterChangeEdit->getPrevOperator()),
                        schedule_consts::logicalOperatorStrings.at(filterChangeEdit->getNewOperator()),
                        i);
                    break;
                }
                case (ScheduleEditType::FilterRuleAddOrRemove):
                {
                    auto filterRuleEdit = std::dynamic_pointer_cast<FilterRuleAddOrRemoveEditBase>(editHistory[i]);
                    sprintf(buf, "%c Filter rule #%zu in filter #%zu in group #%zu of Column %zu##%zu", 
                        filterRuleEdit->getIsRemove() ? '-' : '+', 
                        filterRuleEdit->getFilterRuleIndex(),
                        filterRuleEdit->getFilterIndex(),
                        filterRuleEdit->getFilterGroupIndex(),
                        filterRuleEdit->getColumnIndex(),
                        i);
                    break;
                }
                case (ScheduleEditType::FilterRuleChange):
                {
                    auto ruleChangeEdit = std::dynamic_pointer_cast<FilterRuleChangeEditBase>(editHistory[i]);
                    sprintf(buf, "Edit filter rule #%zu in filter #%zu in group #%zu of Column %zu (%s -> %s)##%zu", 
                        ruleChangeEdit->getFilterRuleIndex(),
                        ruleChangeEdit->getFilterIndex(),
                        ruleChangeEdit->getFilterGroupIndex(),
                        ruleChangeEdit->getColumnIndex(),
                        ruleChangeEdit->getPrevRule().getString().c_str(),
                        ruleChangeEdit->getNewRule().getString().c_str(),
                        i);
                    break;
                }
                default:
                {
                    sprintf(buf, "Edit of unknown type##%zu", i);
                    break;
                }
            }

            if (ImGui::Selectable(buf, m_scheduleEditHistory->getEditHistoryIndex() == i)) 
            {
                while (m_scheduleEditHistory->getEditHistoryIndex() > i)
                {
                    undoEvent.invoke();
                }
                while (m_scheduleEditHistory->getEditHistoryIndex() < i)
                {
                    redoEvent.invoke();
                }
            }
        }
    }
    ImGui::End();
}

void EditHistoryGui::passScheduleEditHistory(const ScheduleEditHistory* editHistory)
{
    m_scheduleEditHistory = editHistory;
}