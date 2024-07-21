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
                    ElementEditBase* elementEdit = (ElementEditBase*)editHistory[i];
                    sprintf(buf, "Element of type %s at (%zu; %zu)##%zu", 
                        schedule_consts::scheduleTypeNames.at(elementEdit->getElementType()), 
                        elementEdit->getPosition().first, 
                        elementEdit->getPosition().second,
                        i);
                    break;
                }
                case (ScheduleEditType::RowAddOrRemove):
                {
                    RowEdit* rowEdit = (RowEdit*)editHistory[i];
                    sprintf(buf, "%c Row at %zu##%zu", 
                        rowEdit->getIsRemove() ? '-' : '+', 
                        rowEdit->getRow(),
                        i);
                    break;
                }
                case (ScheduleEditType::ColumnAddOrRemove):
                {
                    ColumnEdit* columnEdit = (ColumnEdit*)editHistory[i];
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
                    ColumnPropertyEdit* columnPropertyEdit = (ColumnPropertyEdit*)editHistory[i];
                    COLUMN_PROPERTY editedProperty = columnPropertyEdit->getEditedProperty();
                    sprintf(buf, "Column %s property %s at %zu##%zu", 
                        columnPropertyEdit->getColumnName().c_str(),
                        editedProperty == COLUMN_PROPERTY_NAME ? "Name" : (editedProperty == COLUMN_PROPERTY_TYPE ? "Type" : (editedProperty == COLUMN_PROPERTY_SELECT_OPTIONS ? "Select options" : "Sort")),
                        columnPropertyEdit->getColumn(),
                        i);
                    break;
                }
                case (ScheduleEditType::FilterRuleAddOrRemove):
                {
                    FilterRuleAddOrRemoveEditBase* filterRuleEdit = (FilterRuleAddOrRemoveEditBase*)editHistory[i];
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
                    FilterRuleChangeEditBase* ruleChangeEdit = (FilterRuleChangeEditBase*)editHistory[i];
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