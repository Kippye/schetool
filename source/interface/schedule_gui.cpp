
#include <array>
#include <ctime>
#include <iterator>
#include <string>
#include <cstdio>
#include <algorithm>
#include <schedule_constants.h>
#include <schedule_gui.h>
#include <util.h>
#include <element.h>
#include <element_base.h>
#include "filter.h"
#include <schedule.h>
#include "gui_templates.h"
#include <iostream>

void ScheduleGui::passScheduleComponents(const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents)
{
	m_scheduleCore = &scheduleCore;

	addSubGui("ElementEditorSubGui", new ElementEditorSubGui("ElementEditorSubGui", m_scheduleCore));
	addSubGui("FilterEditorSubGui", new FilterEditorSubGui("FilterEditorSubGui", m_scheduleCore, scheduleEvents));
}

void ScheduleGui::draw(Window& window, Input& input)
{
    if (m_visible == false) { return; }

    const float FILTER_SPACE_VERTICAL = 0.0f; // 48.0f;
    const float SCHEDULE_OFFSET = 32.0f; // highestFilterCount * FILTER_SPACE_VERTICAL;
    const float ADD_ROW_BUTTON_HEIGHT = 32.0f;
    const float ADD_COLUMN_BUTTON_WIDTH = 32.0f;
    const float CHILD_WINDOW_WIDTH = (float)(window.SCREEN_WIDTH - ADD_COLUMN_BUTTON_WIDTH - 8);
    const float CHILD_WINDOW_HEIGHT = (float)(window.SCREEN_HEIGHT - SCHEDULE_OFFSET - ADD_ROW_BUTTON_HEIGHT - 16.0f);
    //ImGui::SetNextWindowSizeConstraints(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT), ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
	ImGui::SetNextWindowSize(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

	ImGui::Begin(m_ID.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		// TODO: For the schedule table, combine
		// Reorderable, hideable, with headers & ImGuiTableFlags_ScrollY and background colours and context menus in body and custom headers
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	    ImGui::SetNextWindowPos(ImVec2(0.0, SCHEDULE_OFFSET));
		ImGui::BeginChild("SchedulePanel", ImVec2(CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT), true);
			ImGuiTableFlags tableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_Borders;
            // avoid imgui 0 column abort by not beginning the table at all if there are no columns in the schedule
            if (m_scheduleCore->getColumnCount() == 0)
            {
                goto skip_schedule_table;
            }
			if (ImGui::BeginTable("ScheduleTable", m_scheduleCore->getColumnCount(), tableFlags))
			{ 
				for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
				{
					ImGui::TableSetupColumn(m_scheduleCore->getColumn(column)->name.c_str());
				}
 
                // filters row
                ImGui::TableNextRow();
                for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
                {
                    ImGui::TableSetColumnIndex(column);

                    if (ImGui::SmallButton(std::string("+##addFilter").append(std::to_string(column)).c_str()))
                    {
                        // display the Filter editor to add a filter to this Column
                        if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui"))
                        {
                            filterEditor->open_create(column, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                        }
                    }
                        
                    if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui"))
                    {
                        if (filterEditor->getFilterColumn() == column)
                        {
                            filterEditor->draw(window, input);
                        }
                    }

                    const Column* currentColumn = m_scheduleCore->getColumn(column);

                    ImGui::SameLine();

                    for (size_t i = 0; i < currentColumn->getFilterCount(); i++)
                    {
                        float filterButtonWidth = ImGui::GetColumnWidth(-1) / currentColumn->getFilterCount();
                        if (ImGui::Button(std::string(m_scheduleCore->getColumn(column)->name).append("##").append(std::to_string(i)).c_str(), ImVec2(filterButtonWidth, 0)))
                        {
                            if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui"))
                            {
                                filterEditor->open_edit(column, i, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                            }
                        }

                        if (i < currentColumn->getFilterCount() - 1)
                        {
                            ImGui::SameLine();
                        }
                    }
                }

				// custom header row
				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
				for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
				{
					ImGui::TableSetColumnIndex(column);
					const char* columnName = ImGui::TableGetColumnName(column); // get name passed to TableSetupColumn()
					ImGui::PushID(column);
					// sort button!
					if (ImGui::ArrowButton(std::string("##sortColumn").append(std::to_string(column)).c_str(), m_scheduleCore->getColumn(column)->sort == COLUMN_SORT_NONE ? ImGuiDir_Right : (m_scheduleCore->getColumn(column)->sort == COLUMN_SORT_DESCENDING ? ImGuiDir_Down : ImGuiDir_Up)))
					{
						setColumnSort.invoke(column, m_scheduleCore->getColumn(column)->sort == COLUMN_SORT_NONE ? COLUMN_SORT_DESCENDING : (m_scheduleCore->getColumn(column)->sort == COLUMN_SORT_DESCENDING ? COLUMN_SORT_ASCENDING : COLUMN_SORT_NONE));
					}
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
					// close button
					// permanent columns can't be removed so there's no need for a remove button
					if (m_scheduleCore->getColumn(column)->permanent == false)
					{
						if (ImGui::Button("X##removecolumn", ImVec2(20.0, 20.0)))
						{
							removeColumn.invoke(column);
                            ImGui::PopID();
                            ImGui::EndTable();
                            goto skip_schedule_table;
						}
						ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
					}
					ImGui::TableHeader(columnName);
					// column header context menu
                    if (ImGui::BeginPopupContextItem("#ColumnEdit"))
                    {
						displayColumnContextPopup(column, tableFlags);
                        ImGui::EndPopup();
                    }
					ImGui::PopID();
				}

				std::vector<size_t> sortedRowIndices = m_scheduleCore->getSortedRowIndices();

				for (size_t unsortedRow = 0; unsortedRow < m_scheduleCore->getRowCount(); unsortedRow++)
				{
					size_t row = sortedRowIndices[unsortedRow];
					
                    // CHECK FILTERS BEFORE DRAWING ROW
                    for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
                    {
                        // check if the row's Element passes all Filters in this Column
                        for (const auto& filter: m_scheduleCore->getColumn(column)->getFiltersConst())
                        {
                            // fails to pass, don't show this row                            
                            if (filter->checkPasses(m_scheduleCore->getElementConst(column, row)) == false)
                            {
                                goto do_not_draw_row;
                            }
                        }
                    }

					ImGui::TableNextRow();
					for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
					{
						ImGui::TableSetColumnIndex(column);
						// the buttons for removing rows are displayed in the first displayed column
						if (ImGui::GetCurrentTable()->Columns[column].DisplayOrder == 0)
						{
							if (ImGui::Button(std::string("X##").append(std::to_string(row)).c_str(), ImVec2(26.0, 26.0)))
							{
								removeRow.invoke(row);
								// break because this row can't be drawn anymore, it was removed.
								break;
							}
							ImGui::SameLine();
						}

						SCHEDULE_TYPE columnType = m_scheduleCore->getColumn(column)->type;
						// TODO: i could probably reduce the code repetition here
						ImGui::SetNextItemWidth(-FLT_MIN);
 
						switch(columnType)
						{
							case(SCH_BOOL):
							{
                                bool newValue = m_scheduleCore->getElementValueConstRef<bool>(column, row);
                                if (ImGui::Checkbox(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue))
                                {
                                    setElementValueBool.invoke(column, row, newValue);
                                }
								break;
							}
							case(SCH_NUMBER):
							{
                                int newValue = m_scheduleCore->getElementValueConstRef<int>(column, row);
                                if (ImGui::InputInt(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue, 0, 100, ImGuiInputTextFlags_EnterReturnsTrue))
                                {
                                    setElementValueNumber.invoke(column, row, newValue);
                                }
								break;
							}
							case(SCH_DECIMAL):
							{
                                double newValue = m_scheduleCore->getElementValueConstRef<double>(column, row);
                                if (ImGui::InputDouble(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue, 0.0, 0.0, "%.15g", ImGuiInputTextFlags_EnterReturnsTrue))
                                {
                                    setElementValueDecimal.invoke(column, row, newValue);
                                }
								break;
							}
							case(SCH_TEXT):
							{
                                std::string value = m_scheduleCore->getElementValueConstRef<std::string>(column, row);
                                std::string displayedValue = value;

                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (elementEditor->getOpenThisFrame() && (column == editorColumn && row == editorRow))
                                    {
                                        // if editing this text element, use this TextWrapped as a preview, the value will actually only be applied if the editor's input is applied
                                        displayedValue = elementEditor->getEditorValue(displayedValue);
                                    }
                                }
                                // element to display the value as a wrapped, multiline text
                                ImGui::TextWrapped("%s", displayedValue.c_str());
                                // open text editor if clicked while hovering the current column & row (note: hovered row has an offset removed to account for the filter and header rows)
                                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::TableGetHoveredColumn() == column && ImGui::TableGetHoveredRow() - 2 == row)
                                // if (ImGui::IsItemClicked())
                                {
                                    if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                    {
                                        elementEditor->setEditorValue(value);
                                        elementEditor->setTextInputBoxSize(ImVec2(ImGui::GetColumnWidth(column), 0));
                                        elementEditor->open(column, row, SCH_TEXT, ImRect(ImGui::GetItemRectMin(), ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), column).Max));
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueText.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
							case(SCH_SELECT):
							{
                                SelectContainer value = m_scheduleCore->getElementValueConstRef<SelectContainer>(column, row);
                                auto selection = value.getSelection();
                                const std::vector<std::string>& optionNames = m_scheduleCore->getColumn(column)->selectOptions.getOptions();

                                std::vector<int> selectionIndices = {};

                                // error fix attempt: there should never be more selected that options
                                while (selection.size() > optionNames.size())
                                {
                                    printf("ScheduleGui::draw: Select at (%zu; %zu) has more indices in selection (%zu) than existing options (%zu). Removing selection indices until valid!\n", column, row, selection.size(), optionNames.size());
                                    selection.erase(--selection.end());
                                }

                                for (size_t s: selection)
                                {
                                    // error fix attempt: there should never be selection indices that are >= optionNames.size()
                                    if (s >= optionNames.size())
                                    {
                                        printf("ScheduleGui::draw: Select at (%zu; %zu) index (%zu) >= optionNames.size() (%zu). Removing index from selection.\n", column, row, s, optionNames.size());
                                        selection.erase(s);
                                    }
                                }

                                size_t selectedCount = selection.size();

                                for (size_t s: selection)
                                {
                                    selectionIndices.push_back(s);
                                }

                                // sort indices so that the same options are always displayed in the same order
                                std::sort(std::begin(selectionIndices), std::end(selectionIndices));

                                float displayedChars = 0;
                                float pixelsPerCharacter = 12.0f;
                                float columnWidth = ImGui::GetColumnWidth(column);

                                for (size_t i = 0; i < selectedCount; i++)
                                {
                                    // TODO: colors later ImGui::PushStyleColor(ImGuiCol_Button, m_dayColours[i]);
                                    if (ImGui::ButtonEx(std::string(optionNames[selectionIndices[i]]).append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str(), ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
                                    {
                                        // left clicking opens the editor like the user would expect
                                        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                                        {
                                            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                            {
                                                elementEditor->open(column, row, SCH_SELECT, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                                elementEditor->setEditorValue(value);
                                            }
                                        }
                                        // right clicking erases the option - bonus feature
                                        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                                        {
                                            value.setSelected(selectionIndices[i], false);
                                            setElementValueSelect.invoke(column, row, value); 
                                        }
                                    }
                                    // HACK to make this show when any of the options is hovered
                                    if (i != selectedCount - 1 && ImGui::IsItemHovered())
                                    {
                                        ImGui::BeginTooltip();
                                        ImGui::Text("Created: %s %s", m_scheduleCore->getElementConst(column, row)->getCreationDate().getString().c_str(), m_scheduleCore->getElementConst(column, row)->getCreationTime().getString().c_str());
                                        ImGui::EndTooltip();
                                    }

                                    displayedChars += optionNames[selectionIndices[i]].length();
                                    if (i < selectedCount - 1 && floor(displayedChars * pixelsPerCharacter / columnWidth) == floor((displayedChars - optionNames[selectionIndices[i]].length()) * pixelsPerCharacter / columnWidth))
                                    {
                                        ImGui::SameLine();
                                    }
                                    // ImGui::PopStyleColor(1);
                                }

                                // TEMP ? if there are no options selected, just show an "Edit" button to prevent kind of a softlock
                                if (selectedCount == 0)
                                {
                                    if (ImGui::Button(std::string("Edit##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
                                    {
                                        if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                        {
                                            elementEditor->open(column, row, SCH_SELECT, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                            elementEditor->setEditorValue(value);
                                        }
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueSelect.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
                            case(SCH_WEEKDAY):
							{
                                WeekdayContainer value = m_scheduleCore->getElementValueConstRef<WeekdayContainer>(column, row);
                                auto selection = value.getSelection();
                                const std::vector<std::string>& optionNames = schedule_consts::weekdayNames;

                                std::vector<int> selectionIndices = {};

                                // error fix attempt: there should never be more selected that options
                                while (selection.size() > 7)
                                {
                                    printf("ScheduleGui::draw: WeekdayContainer at (%zu; %zu) has more indices in selection (%zu) than existing options (%d). Removing selection indices until valid!\n", column, row, selection.size(), 7);
                                    selection.erase(--selection.end());
                                }

                                for (size_t s: selection)
                                {
                                    // error fix attempt: there should never be selection indices that are >= optionNames.size()
                                    if (s >= 7)
                                    {
                                        printf("ScheduleGui::draw: WeekdayContainer at (%zu; %zu) index (%zu) >= optionNames.size() (%d). Removing index from selection.\n", column, row, s, 7);
                                        selection.erase(s);
                                    }
                                }

                                size_t selectedCount = selection.size();

                                for (size_t s: selection)
                                {
                                    selectionIndices.push_back(s);
                                }

                                // sort indices so that the same options are always displayed in the same order
                                std::sort(std::begin(selectionIndices), std::end(selectionIndices));

                                float displayedChars = 0;
                                float pixelsPerCharacter = 12.0f;
                                float columnWidth = ImGui::GetColumnWidth(column);

                                for (size_t i = 0; i < selectedCount; i++)
                                {
                                    ImGui::PushStyleColor(ImGuiCol_Button, gui_colors::dayColors[selectionIndices[i]]);
                                    if (ImGui::ButtonEx(std::string(optionNames[selectionIndices[i]]).append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str(), ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
                                    {
                                        // left clicking opens the editor like the user would expect
                                        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                                        {
                                            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                            {
                                                elementEditor->open(column, row, SCH_WEEKDAY, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                                elementEditor->setEditorValue(value);
                                            }
                                        }
                                        // right clicking erases the option - bonus feature
                                        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                                        {
                                            value.setSelected(selectionIndices[i], false);
                                            setElementValueSelect.invoke(column, row, value); 
                                        }
                                    }
                                    // HACK to make this show when any of the options is hovered
                                    if (i != selectedCount - 1 && ImGui::IsItemHovered())
                                    {
                                        ImGui::BeginTooltip();
                                        ImGui::Text("Created: %s %s", m_scheduleCore->getElementConst(column, row)->getCreationDate().getString().c_str(), m_scheduleCore->getElementConst(column, row)->getCreationTime().getString().c_str());
                                        ImGui::EndTooltip();
                                    }

                                    displayedChars += optionNames[selectionIndices[i]].length();
                                    if (i < selectedCount - 1 && floor(displayedChars * pixelsPerCharacter / columnWidth) == floor((displayedChars - optionNames[selectionIndices[i]].length()) * pixelsPerCharacter / columnWidth))
                                    {
                                        ImGui::SameLine();
                                    }
                                    ImGui::PopStyleColor(1);
                                }

                                // TEMP ? if there are no options selected, just show an "Edit" button to prevent kind of a softlock
                                if (selectedCount == 0)
                                {
                                    if (ImGui::Button(std::string("Edit##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
                                    {
                                        if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                        {
                                            elementEditor->open(column, row, SCH_WEEKDAY, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                            elementEditor->setEditorValue(value);
                                        }
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueWeekday.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
							case(SCH_TIME):
							{
                                TimeContainer value = m_scheduleCore->getElementValueConstRef<TimeContainer>(column, row);

                                // Button displaying the Time of the current Time element
                                if (ImGui::Button(value.getString().append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
                                {
                                    if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                    {
                                        elementEditor->setEditorValue(value);
                                        elementEditor->open(column, row, SCH_TIME, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueTime.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
							case(SCH_DATE):
							{
                                auto value = m_scheduleCore->getElementValueConstRef<DateContainer>(column, row);
                            
                                // Button displaying the date of the current Date element
                                if (ImGui::Button(value.getString().append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
                                {
                                    if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                    {
                                        elementEditor->setEditorValue(value);
                                        elementEditor->open(column, row, SCH_DATE, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueDate.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("Created: %s %s", m_scheduleCore->getElementConst(column, row)->getCreationDate().getString().c_str(), m_scheduleCore->getElementConst(column, row)->getCreationTime().getString().c_str());
							ImGui::EndTooltip();
						}
					}
                    // END OF for (size_t unsortedRow = 0; unsortedRow < m_scheduleCore->getRowCount(); unsortedRow++)
                    do_not_draw_row:
                    bool b = false; // stupid thing because fsr the label can't be at the end of the loop
				}
				ImGui::EndTable();
			}
        skip_schedule_table:
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::Button("+", ImVec2(ADD_COLUMN_BUTTON_WIDTH, (float)(CHILD_WINDOW_HEIGHT))))
		{
			addDefaultColumn.invoke(m_scheduleCore->getColumnCount());
		}
		if (ImGui::Button("Add row", ImVec2((float)(window.SCREEN_WIDTH - ADD_COLUMN_BUTTON_WIDTH - 26), ADD_ROW_BUTTON_HEIGHT)))
		{
			addRow.invoke(m_scheduleCore->getRowCount());
		}
		ImGui::PopStyleVar();
    ImGui::End();
}

void ScheduleGui::displayColumnContextPopup(unsigned int column, ImGuiTableFlags tableFlags)
{
	// FIXME TODO STUPID HACK !!
	ImGuiTable* table = ImGui::GetCurrentContext()->Tables.GetByIndex(0) ;
	//ImGui::TableFindByID(ImGui::GetID("ScheduleTable"));

	// renaming
	std::string name = m_scheduleCore->getColumn(column)->name.c_str();
	name.reserve(COLUMN_NAME_MAX_LENGTH);
	char* buf = name.data();
	
	if (ImGui::InputText(std::string("##columnName").append(std::to_string(column)).c_str(), buf, name.capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		setColumnName.invoke(column, buf);
	}

	// select type (for non-permanent columns)
	if (m_scheduleCore->getColumn(column)->permanent == false)
	{
		ImGui::Separator();
		SCHEDULE_TYPE selected = m_scheduleCore->getColumn(column)->type;
		for (unsigned int i = 0; i < (unsigned int)SCH_LAST; i++)
		{
			if (ImGui::Selectable(schedule_consts::scheduleTypeNames.at((SCHEDULE_TYPE)i), selected == (SCHEDULE_TYPE)i))
				setColumnType.invoke(column, SCHEDULE_TYPE(i));
		}
	}

	ImGui::Separator(); 
	
	// resizing
	if (tableFlags & ImGuiTableFlags_Resizable)
	{
		if (ImGui::MenuItem("Size column to fit###SizeOne", NULL, false))
			ImGui::TableSetColumnWidthAutoSingle(table, column);

		const char* size_all_desc;
		//if (table->ColumnsEnabledFixedCount == table->ColumnsEnabledCount && (table->Flags & ImGuiTableFlags_SizingMask_) != ImGuiTableFlags_SizingFixedSame)
		//	size_all_desc = "Size all columns to fit###SizeAll";        // All fixed
		//else
		size_all_desc = "Size all columns to default###SizeAll";    // All stretch or mixed
		if (ImGui::MenuItem(size_all_desc, NULL))
			ImGui::TableSetColumnWidthAutoAll(table);
	}

	// Ordering
	if (tableFlags & ImGuiTableFlags_Reorderable)
	{
		if (ImGui::MenuItem("Reset order", NULL, false, !table->IsDefaultDisplayOrder))
			table->IsResetDisplayOrderRequest = true;
	}

	ImGui::Separator();

	// Hiding / Visibility
	if (tableFlags & ImGuiTableFlags_Hideable)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
		for (int otherColumnIndex = 0; otherColumnIndex < table->ColumnsCount; otherColumnIndex++)
		{
			if (m_scheduleCore->getColumn(otherColumnIndex)->permanent)
			{
				continue;
			}
			ImGuiTableColumn* otherColumn = &table->Columns[otherColumnIndex];
			const char* name = ImGui::TableGetColumnName(table, otherColumnIndex);
			if (name == NULL || name[0] == 0)
				name = "Unnamed";

			// Make sure we can't hide the last active column
			bool menu_item_active = (otherColumn->Flags & ImGuiTableColumnFlags_NoHide) ? false : true;
			if (otherColumn->IsEnabled && table->ColumnsEnabledCount <= 1)
				menu_item_active = false;
			if (ImGui::MenuItem(name, NULL, otherColumn->IsEnabled, menu_item_active))
				otherColumn->IsUserEnabledNextFrame = !otherColumn->IsEnabled;
		}
		ImGui::PopItemFlag();
	}

	if (ImGui::Button("Close"))
		ImGui::CloseCurrentPopup();
}