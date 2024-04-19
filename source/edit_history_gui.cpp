#include "imgui.h"
#include "schedule_edit.h"
#include <cstdio>
#include <edit_history_gui.h>

EditHistoryGui::EditHistoryGui(const char* ID, Schedule* schedule) : Gui(ID)
{
    m_schedule = schedule;
}

void EditHistoryGui::draw(Window& window, Input& input)
{
    if (ImGui::Begin("Edit History", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        auto editHistory = m_schedule->getEditHistory().getEditHistory();

        for (size_t i = 0; i < editHistory.size(); i++)
        {
            std::string s;
            s.reserve(256);
            char* buf = s.data();

            switch (editHistory[i]->getType())
            {
                case (SCHEDULE_EDIT_ELEMENT):
                {
                    ElementEditBase* elementEdit = (ElementEditBase*)editHistory[i];
                    sprintf(buf, "Element of type %s at (%zu; %zu)", 
                        m_schedule->scheduleTypeNames.at(elementEdit->getElementType()), 
                        elementEdit->getPosition().first, 
                        elementEdit->getPosition().second);
                    break;
                }
                case (SCHEDULE_EDIT_ROW):
                {
                    RowEdit* rowEdit = (RowEdit*)editHistory[i];
                    sprintf(buf, "%c Row at %zu", 
                        rowEdit->getIsRemove() ? '-' : '+', 
                        rowEdit->getRow());
                    break;
                }
                case (SCHEDULE_EDIT_COLUMN):
                {
                    ColumnEdit* columnEdit = (ColumnEdit*)editHistory[i];
                    sprintf(buf, "%c Column %s of type %s at %zu", 
                        columnEdit->getIsRemove() ? '-' : '+',
                        columnEdit->getColumnData().name.c_str(), 
                        m_schedule->scheduleTypeNames.at(columnEdit->getColumnData().type),
                        columnEdit->getColumn());
                    break;
                }
                case (SCHEDULE_EDIT_COLUMN_PROPERTY):
                {
                    ColumnPropertyEdit* columnPropertyEdit = (ColumnPropertyEdit*)editHistory[i];
                    COLUMN_PROPERTY editedProperty = columnPropertyEdit->getEditedProperty();
                    sprintf(buf, "Column %s property %s at %zu", 
                        columnPropertyEdit->getColumnName().c_str(),
                        editedProperty == COLUMN_PROPERTY_NAME ? "Name" : (editedProperty == COLUMN_PROPERTY_TYPE ? "Type" : (editedProperty == COLUMN_PROPERTY_SELECT_OPTIONS ? "Select options" : "Sort")),
                        columnPropertyEdit->getColumn());
                    break;
                }
            }

            ImGui::Selectable(buf, m_schedule->getEditHistory().getEditHistoryIndex() == i);
        }
    }
    ImGui::End();
}