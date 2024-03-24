#include "decimal_container.h"
#include "element.h"
#include <select_container.h>
#include <algorithm>
#include <array>
#include <cctype>
#include <ctime>
#include <iterator>
#include <schedule_gui.h>
#include <schedule.h>
#include <imgui.h>
#include <string>
#include <window.h>
#include <any>
#include <cstdio>
#include <regex>
#include <util.h>

#include <iostream>

ScheduleGui::ScheduleGui(const char* ID, Schedule* schedule)
{
	m_ID = ID;
	m_schedule = schedule;
} 

void ScheduleGui::draw(Window& window)
{
    ImGui::SetNextWindowSizeConstraints(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT), ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
	ImGui::SetNextWindowSize(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
	ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));

	ImGui::Begin(m_ID.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
		// TODO: For the schedule table, combine
		// Reorderable, hideable, with headers & ImGuiTableFlags_ScrollY and background colours and context menus in body and custom headers
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("SchedulePanel", ImVec2((float)(window.SCREEN_WIDTH - 58), (float)(window.SCREEN_HEIGHT - 52)), true);
			ImGuiTableFlags tableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableRowFlags_Headers | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_Borders;
			ImGui::BeginTable("ScheduleTable", m_schedule->getColumnCount(), tableFlags);
				for (size_t column = 0; column < m_schedule->getColumnCount(); column++)
				{
					ImGui::TableSetupColumn(m_schedule->getColumn(column)->name.c_str());
				}
				// custom header row
				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
				for (size_t column = 0; column < m_schedule->getColumnCount(); column++)
				{
					ImGui::TableSetColumnIndex(column);
					const char* columnName = ImGui::TableGetColumnName(column); // get name passed to TableSetupColumn()
					ImGui::PushID(column);
					// sort button!
					if (ImGui::ArrowButton(std::string("##sortColumn").append(std::to_string(column)).c_str(), m_schedule->getColumn(column)->sort == COLUMN_SORT_NONE ? ImGuiDir_Right : (m_schedule->getColumn(column)->sort == COLUMN_SORT_DESCENDING ? ImGuiDir_Down : ImGuiDir_Up)))
					{
						m_schedule->setColumnSort(column, m_schedule->getColumn(column)->sort == COLUMN_SORT_NONE ? COLUMN_SORT_DESCENDING : (m_schedule->getColumn(column)->sort == COLUMN_SORT_DESCENDING ? COLUMN_SORT_ASCENDING : COLUMN_SORT_NONE));
					}
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
					// close button
					// permanent columns can't be removed so there's no need for a remove button
					if (m_schedule->getColumn(column)->permanent == false)
					{
						if (ImGui::Button("X##removecolumn", ImVec2(20.0, 20.0)))
						{
							m_schedule->removeColumn(column);
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

				for (size_t row = 0; row < m_schedule->getRowCount(); row++)
				{
					ImGui::TableNextRow();
					for (size_t column = 0; column < m_schedule->getColumnCount(); column++)
					{
						ImGui::TableSetColumnIndex(column);
						// the buttons for removing rows are displayed in the first displayed column
						if (ImGui::GetCurrentTable()->Columns[column].DisplayOrder == 0)
						{
							if (ImGui::Button(std::string("X##").append(std::to_string(row)).c_str(), ImVec2(26.0, 26.0)))
							{
								m_schedule->removeRow(row);
								// break because this row can't be drawn anymore, it was removed.
								break;
							}
							ImGui::SameLine();
						}

						SCHEDULE_TYPE columnType = m_schedule->getColumn(column)->type;
						// TODO: i could probably reduce the code repetition here
						ImGui::SetNextItemWidth(-FLT_MIN);
 
						switch(columnType)
						{
							case(SCH_BOOL):
							{
								try
								{
									Bool container = m_schedule->getElement<Bool>(column, row);
									bool newValue = container.getValue();
									if (ImGui::Checkbox(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue))
									{
										container.setValue(newValue);
										m_schedule->setElement<Bool>(column, row, new Bool(container));
									}
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}
								break;
							}
							case(SCH_NUMBER):
							{
								try
								{
									Number container = m_schedule->getElement<Number>(column, row);
									int newValue = container.getValue();
									ImGui::InputInt(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue, 0, 100, ImGuiInputTextFlags_EnterReturnsTrue);
									container.setValue(newValue);
									m_schedule->setElement<Number>(column, row, new Number(container));
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}
								break;
							}
							case(SCH_DECIMAL):
							{
								try
								{
									Decimal container = m_schedule->getElement<Decimal>(column, row);
									double newValue = container.getValue();
									ImGui::InputDouble(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue);
									container.setValue(newValue);
									m_schedule->setElement<Decimal>(column, row, new Decimal(container));
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}
								
								break;
							}
							case(SCH_TEXT):
							{
								try
								{
									Text container = m_schedule->getElement<Text>(column, row);
									std::string value = container.getValue();
									value.reserve(ELEMENT_TEXT_MAX_LENGTH);
									char* buf = value.data();
									//ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
									if (ImGui::InputTextMultiline(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity(), ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
									{
										container.setValue(std::string(buf));
										m_schedule->setElement<Text>(column, row, new Text(container));
									}
									// ImVec2 textSize = ImGui::CalcTextSize(buf);
									// if (textSize.x == 106 && textSize.y == 16)
									// {
									// 	ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetItemID());
									// 	std::cout << "newline" << std::endl;
									// 	state->OnKeyPressed((int)'\n');
									// 	std::cout << buf << std::endl;
									// }
									// std::cout << textSize.x << "; " << textSize.y << std::endl;
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}
								
								break;
							}
							case(SCH_SELECT):
							{
								try 
								{
									Select value = m_schedule->getElement<Select>(column, row);
									auto selection = value.getSelection();
									size_t selectedCount = selection.size();
									const std::vector<std::string>& optionNames = m_schedule->getColumn(column)->selectOptions.getOptions();

									std::vector<int> selectionIndices = {};

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
												m_editorColumn = column;
												m_editorRow = row;
												m_editorSelect = value;
												m_editorAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
												ImGui::OpenPopup("Editor");
											}
											// right clicking erases the option - bonus feature
											else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
											{
												value.setSelected(selectionIndices[i], false);
												m_schedule->setElement<Select>(column, row, new Select(value));
											}
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
											m_editorColumn = column;
											m_editorRow = row;
											m_editorSelect = value;
											m_editorAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
											ImGui::OpenPopup("Editor");
										}
									}
									if (column == m_editorColumn && row == m_editorRow)
									{
										displayEditor(SCH_SELECT);
										if (m_editorOpenLastFrame == true && m_editorOpenThisFrame == false)
										{
											m_schedule->setElement<Select>(column, row, new Select(m_editorSelect));
										}
									}
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}

								break;
							}
							case(SCH_TIME):
							{
								try
								{
									Time value = m_schedule->getElement<Time>(column, row);

									// Button displaying the Time of the current Time element
									if (ImGui::Button(value.getString().append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
									{
										m_editorColumn = column;
										m_editorRow = row;
										m_editorTime = value;
										m_editorAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
										ImGui::OpenPopup("Editor");
									}
									if (column == m_editorColumn && row == m_editorRow)
									{
										displayEditor(SCH_TIME);
										if (m_editorOpenLastFrame == true && m_editorOpenThisFrame == false)
										{
											m_schedule->setElement<Time>(column, row, new Time(m_editorTime));
										}
									}
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}
								
								break;
							}
							case(SCH_DATE):
							{
								try
								{
									auto value = m_schedule->getElement<Date>(column, row);
								
									// Button displaying the date of the current Date element
									if (ImGui::Button(value.getString().append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
									{
										m_editorColumn = column;
										m_editorRow = row;
										m_editorDate = value;
										m_editorViewedMonth = m_editorDate.getTime()->tm_mon;
										m_editorViewedYear = m_editorDate.getTime()->tm_year;
										m_editorAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
										ImGui::OpenPopup("Editor");
									}
									if (column == m_editorColumn && row == m_editorRow)
									{
										displayEditor(SCH_DATE);
										if (m_editorOpenLastFrame == true && m_editorOpenThisFrame == false)
										{
											m_schedule->setElement<Date>(column, row, new Date(m_editorDate));
										}
									}
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}
								
								break;
							}
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							std::cout << m_schedule->getElement<Element>(column, row).getCreationDate().time.tm_mday << std::endl;
							ImGui::Text("Created: %s %s", m_schedule->getElement<Element>(column, row).getCreationDate().getString().c_str(), m_schedule->getElement<Element>(column, row).getCreationTime().getString().c_str());
							ImGui::EndTooltip();
						}
					}
				}
			ImGui::EndTable();
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::Button("+", ImVec2(32, (float)(window.SCREEN_HEIGHT - 52))))
		{
			m_schedule->addDefaultColumn(m_schedule->getColumnCount());
		}
		if (ImGui::Button("Add row", ImVec2((float)(window.SCREEN_WIDTH - 58), 32)))
		{
			m_schedule->addRow(m_schedule->getRowCount());
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
	std::string name = m_schedule->getColumn(column)->name.c_str();
	name.reserve(COLUMN_NAME_MAX_LENGTH);
	char* buf = name.data();
	
	ImGui::InputText(std::string("##columnName").append(std::to_string(column)).c_str(), buf, name.capacity());
	m_schedule->setColumnName(column, buf);

	// select type (for non-permanent columns)
	if (m_schedule->getColumn(column)->permanent == false)
	{
		ImGui::Separator();
		SCHEDULE_TYPE selected = m_schedule->getColumn(column)->type;
		for (unsigned int i = 0; i < (unsigned int)SCH_LAST; i++)
		{
			if (ImGui::Selectable(m_schedule->scheduleTypeNames.at((SCHEDULE_TYPE)i), selected == (SCHEDULE_TYPE)i))
				m_schedule->setColumnType(column, SCHEDULE_TYPE(i));
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
			if (m_schedule->getColumn(otherColumnIndex)->permanent)
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

// Display the time editor popup's contents. The returned bool indicates whether any of the elements were edited.
bool ScheduleGui::displayEditor(SCHEDULE_TYPE type)
{
	bool valueEdited = false;
	// give old current open state to the last frame's state
	m_editorOpenLastFrame = m_editorOpenThisFrame;

	if (ImGui::BeginPopupEx(ImGui::GetID("Editor"), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
	{
		// TODO clean up / make the positioning more precise!
		ImGuiDir dir = ImGuiDir_Down;
		ImGuiWindow* popup = ImGui::GetCurrentWindow();
		ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
		ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
		ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(popup->Pos, autoFitSize, &popup->AutoPosLastDirection, r_outer, m_editorAvoidRect, ImGuiPopupPositionPolicy_Default));
		//return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, ImRect(window->Pos, window->Pos), ImGuiPopupPositionPolicy_Default); // Ideally we'd disable r_avoid here

		bool isPermanentColumn = m_schedule->getColumn(m_editorColumn)->permanent;
		tm formatTime;
		
		switch(type)
		{
			case(SCH_TIME):
			{
				// TIME
				formatTime.tm_hour = m_editorTime.getHours();
				char hourBuf[3];
				std::strftime(hourBuf, sizeof(hourBuf), "%H", &formatTime);
				ImGui::SetNextItemWidth(24);
				if (ImGui::InputText("##EditTimeHours", hourBuf, 3, ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll, filterNumbers))
				{
					int hourValue = 0;
					if (std::regex_match(hourBuf, std::regex("[0-9]+")))
					{
						hourValue = std::stoi(hourBuf);
					}
					m_editorTime.setTime(hourValue, m_editorTime.getMinutes());
					valueEdited = true;
				}
				ImGui::SameLine();
				formatTime.tm_min = m_editorTime.getMinutes();
				char minBuf[3];
				std::strftime(minBuf, sizeof(minBuf), "%M", &formatTime);
				ImGui::SetNextItemWidth(24);
				if (ImGui::InputText("##Minutes", minBuf, 3, ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll, filterNumbers))
				{
					int minValue = 0;
					if (std::regex_match(minBuf, std::regex("[0-9]+")))
					{
						minValue = std::stoi(minBuf);
					}
					m_editorTime.setTime(m_editorTime.getHours(), minValue);
					valueEdited = true;
				}

				break;
			}
			case(SCH_DATE):
			{
				ImGui::Text("%s", m_editorDate.getString().c_str());
				// DATE / CALENDAR
				if (ImGui::ArrowButton("##PreviousMonth", ImGuiDir_Left))
				{
					m_editorViewedMonth = m_editorViewedMonth == 0 ? 11 : m_editorViewedMonth - 1;
				}
				ImGui::SameLine();
				char monthName[16];
				formatTime.tm_mon = m_editorViewedMonth;
				std::strftime(monthName, sizeof(monthName), "%B", &formatTime);
				ImGui::Button(std::string(monthName).append(std::string("##Month")).c_str(), ImVec2(96, 0));
				ImGui::SameLine();
				if (ImGui::ArrowButton("##NextMonth", ImGuiDir_Right))
				{
					m_editorViewedMonth = m_editorViewedMonth == 11 ? 0 : m_editorViewedMonth + 1;
				}
				int yearInput = m_editorViewedYear + 1900;
				if (ImGui::InputInt("##YearInput", &yearInput, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					m_editorViewedYear = Date::convertToValidYear(yearInput);
				}

				unsigned int daysInMonth = mytime::get_month_day_count(m_editorViewedYear, m_editorViewedMonth);

				for (unsigned int day = 1; day <= daysInMonth; day++)
				{
					if (ImGui::Button(std::to_string(day).c_str(), ImVec2(24, 24)))
					{
						m_editorDate.setYear(m_editorViewedYear, false);
						m_editorDate.setMonth(m_editorViewedMonth);
						m_editorDate.setMonthDay(day); 
						valueEdited = true;
					}
					if (day == m_editorDate.getTime()->tm_mday)
					{
						// TODO: Highlight this day as selected in the calendar
					}
					if (day % 7 != 0)
					{
						ImGui::SameLine();
					}
				}
				
				break;
			}
			case(SCH_SELECT):
			{
				auto selection = m_editorSelect.getSelection();
				size_t selectedCount = selection.size();
				const std::vector<std::string>& optionNames = m_schedule->getColumnSelectOptions(m_editorColumn).getOptions();

				std::vector<size_t> selectionIndices = {};

				for (size_t s: selection)
				{
					selectionIndices.push_back(s);
				}

				// sort indices so that the same options are always displayed in the same order
				std::sort(std::begin(selectionIndices), std::end(selectionIndices));
				
				for (size_t i = 0; i < selectedCount; i++)
				{
					// TODO: colors later ImGui::PushStyleColor(ImGuiCol_Button, m_dayColours[i]);
					if (ImGui::ButtonEx(std::string(optionNames[selectionIndices[i]]).append("##EditorSelectedOption").append(std::to_string(i)).c_str(), ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
					{
						m_editorSelect.setSelected(selectionIndices[i], false);
					}
					ImGui::SameLine();
					// ImGui::PopStyleColor(1);
				}

				// add new options
				if (m_schedule->getColumnSelectOptions(m_editorColumn).getIsMutable())
				{
					std::string str;
					str.reserve(SELECT_OPTION_NAME_MAX_LENGTH);
					char* buf = str.data();
					//ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
					if (ImGui::InputText("##EditorOptionInput", buf, SELECT_OPTION_NAME_MAX_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
					{
						if (std::string(buf).empty() == false)
						{
							m_schedule->getColumnSelectOptions(m_editorColumn).addOption(std::string(buf));
							m_editorSelect.setSelected(m_schedule->getColumnSelectOptions(m_editorColumn).getOptions().size() - 1, true);
							// NOTE: break here because otherwise the start and end of the function kind of go out of sync
							break;
						}
					}
				}

				for (size_t i = 0; i < optionNames.size(); i++)
				{
					bool selected = selection.find(i) != selection.end();

					if (m_schedule->getColumnSelectOptions(m_editorColumn).getIsMutable())
					{
						if (ImGui::SmallButton(std::string("X##").append(std::to_string(i)).c_str()))
						{
							m_schedule->getColumnSelectOptions(m_editorColumn).removeOption(i);
							m_editorSelect.update(); // update the m_editorSelect separately!
							m_schedule->updateColumnSelects(m_editorColumn);
							// break because the whole thing must be restarted now
							break;
						}
						ImGui::SameLine();
					}

					std::string optionName = std::string(optionNames[i]);

					if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
					{
						m_editorSelect.setSelected(i, selected);
						valueEdited = true;
					}

					// drag to reorder options
					if (m_schedule->getColumnSelectOptions(m_editorColumn).getIsMutable())
					{
						if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
						{
							size_t i_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
							if (i_next >= 0 && i_next < optionNames.size())
							{
								m_schedule->getColumnSelectOptions(m_editorColumn).moveOption(i, i_next);
								m_editorSelect.update();
								m_schedule->updateColumnSelects(m_editorColumn);
								ImGui::ResetMouseDragDelta();
								break;
							}
						}
					}
				}

				break;
			}
			default: return false;
		}

		m_editorOpenThisFrame = true;
		ImGui::EndPopup();
	}
	else
	{
		m_editorOpenThisFrame = false;
	}

	return valueEdited;
}

int ScheduleGui::filterNumbers(ImGuiInputTextCallbackData* data)
{
	if (data->EventChar > 47 && data->EventChar < 58)
		return 0;
	return 1;
}