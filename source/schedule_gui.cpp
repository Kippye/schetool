#include <algorithm>
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
    ImGui::SetNextWindowSizeConstraints(ImVec2(window.SCREEN_WIDTH, window.SCREEN_HEIGHT), ImVec2(window.SCREEN_WIDTH, window.SCREEN_HEIGHT));
	ImGui::SetNextWindowSize(ImVec2(window.SCREEN_WIDTH, window.SCREEN_HEIGHT));
	ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));

	ImGui::Begin(m_ID.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
		// TODO: For the schedule table, combine
		// Reorderable, hideable, with headers & ImGuiTableFlags_ScrollY and background colours and context menus in body and custom headers
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("SchedulePanel", ImVec2(window.SCREEN_WIDTH - 58, window.SCREEN_HEIGHT - 52), true);
			ImGuiTableFlags tableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableRowFlags_Headers | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_Borders;
			ImGui::BeginTable("ScheduleTable", m_schedule->getColumnCount(), tableFlags);
				for (int column = 0; column < m_schedule->getColumnCount(); column++)
				{
					ImGui::TableSetupColumn(m_schedule->getColumnName(column));
				}
				// custom header row
				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
				for (int column = 0; column < m_schedule->getColumnCount(); column++)
				{
					ImGui::TableSetColumnIndex(column);
					const char* columnName = ImGui::TableGetColumnName(column); // get name passed to TableSetupColumn()
					ImGui::PushID(column);
					// close button
					// permanent columns can't be removed so there's no need for a remove button
					if (m_schedule->getColumnPermanent(column) == false)
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
						// FIXME TODO STUPID HACK !!
						ImGuiTable* table = ImGui::GetCurrentContext()->Tables.GetByIndex(0) ;
						//ImGui::TableFindByID(ImGui::GetID("ScheduleTable"));

						// renaming
						std::string name = m_schedule->getColumnName(column);
						name.reserve(COLUMN_NAME_MAX_LENGTH);
						char* buf = name.data();
						
                        ImGui::InputText(std::string("##columnName").append(std::to_string(column)).c_str(), buf, name.capacity());
						m_schedule->setColumnName(column, buf);

						// select type (for non-permanent columns)
						if (m_schedule->getColumnPermanent(column) == false)
						{
							ImGui::Separator();
							SCHEDULE_TYPE selected = m_schedule->getColumnType(column);
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
								if (m_schedule->getColumnPermanent(otherColumnIndex))
								{
									continue;
								}
								ImGuiTableColumn* otherColumn = &table->Columns[otherColumnIndex];
								const char* name = ImGui::TableGetColumnName(table, otherColumnIndex);
								if (name == NULL || name[0] == 0)
									name = "<Unknown>";

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
                        ImGui::EndPopup();
                    }
					ImGui::PopID();
				}

				for (int row = 0; row < m_schedule->getRowCount(); row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < m_schedule->getColumnCount(); column++)
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

						SCHEDULE_TYPE columnType = m_schedule->getColumnType(column);
						// TODO: i could probably reduce the code repetition here
						ImGui::SetNextItemWidth(-FLT_MIN);
						switch(columnType)
						{
							case(SCH_BOOL):
							{
								try
								{
									bool boolValue = bool(m_schedule->getElement<std::byte>(column, row));
									ImGui::Checkbox(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &boolValue);
									m_schedule->setElement<std::byte>(column, row, std::byte(boolValue));
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}
								break;
							}
							case(SCH_INT):
							{
								try
								{
									auto value = m_schedule->getElement<int>(column, row);
									ImGui::InputInt(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &value, 0);
									m_schedule->setElement<int>(column, row, value);
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}
								break;
							}
							case(SCH_DOUBLE):
							{
								try
								{
									auto value = m_schedule->getElement<double>(column, row);
									ImGui::InputDouble(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &value);
									m_schedule->setElement<double>(column, row, value);
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
									std::string value = m_schedule->getElement<std::string>(column, row);
									value.reserve(ELEMENT_TEXT_MAX_LENGTH);
									char* buf = value.data();
									//ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
									ImGui::InputTextMultiline(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity(), ImVec2(0, 0));
									// ImVec2 textSize = ImGui::CalcTextSize(buf);
									// if (textSize.x == 106 && textSize.y == 16)
									// {
									// 	ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetItemID());
									// 	std::cout << "newline" << std::endl;
									// 	state->OnKeyPressed((int)'\n');
									// 	std::cout << buf << std::endl;
									// }
									// std::cout << textSize.x << "; " << textSize.y << std::endl;
									m_schedule->setElement<std::string>(column, row, std::string(buf));
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
									auto value = m_schedule->getElement<Time>(column, row);
									if (m_schedule->getColumnDisplayWeekday(column))
									{
										const bool* selectedWeekdays = value.getSelectedWeekdays();
										unsigned int selectedWeekdayCount = 0;
										
										for (int i = 0; i < 7; i++)
										{
											if (selectedWeekdays[i] == true) { selectedWeekdayCount += 1; }
										}

										std::vector<std::string> displayedWeekdayNames = containers::split(value.getString(m_schedule->getColumnDisplayDate(column), m_schedule->getColumnDisplayTime(column), m_schedule->getColumnDisplayWeekday(column)), ' ');
										unsigned int weekdayNameIndex = 0;

										for (int i = 0; i < 7; i++)
										{
											if (selectedWeekdays[i] == true)
											{
												if (weekdayNameIndex % 3 != 0)
												{
													ImGui::SameLine();
												}
												ImGui::PushStyleColor(ImGuiCol_Button, m_dayColours[i]);
											 	if (ImGui::ButtonEx(displayedWeekdayNames[weekdayNameIndex].append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str(), ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
												{
													// left clicking opens the time editor like the user would expect
													if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
													{
														m_timeEditorColumn = column;
														m_timeEditorRow = row;
														ImGui::OpenPopup("Test#TimeEditor");
													}
													// right clicking erases the day - bonus feature
													else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && selectedWeekdayCount > 1)
													{
														value.setWeekdaySelected(i, !selectedWeekdays[i]);
														m_schedule->setElement<Time>(column, row, value);
													}
												}
												ImGui::PopStyleColor(1);
												weekdayNameIndex++;
											}
										}
									}
									else
									{
										// Button displaying the date, time or selected weekdays of the current Time element
										if (ImGui::Button(value.getString(m_schedule->getColumnDisplayDate(column), m_schedule->getColumnDisplayTime(column), m_schedule->getColumnDisplayWeekday(column)).append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
										{
											m_timeEditorColumn = column;
											m_timeEditorRow = row;
											ImGui::OpenPopup("Test#TimeEditor");
										}
									}
									if (column == m_timeEditorColumn && row == m_timeEditorRow && ImGui::BeginPopupEx(ImGui::GetID("Test#TimeEditor"), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration))
									{
										// display popup contents. if any fields were edited, copy the value to the Schedule.
										if (displayTimeEditor(value))
										{
											// set the actual value in the schedule, because getElement passes element *values*, not pointers or references. maybe this should be changed?
											m_schedule->setElement<Time>(column, row, value);
										}
										ImGui::EndPopup();
									}
								}
								catch(const std::exception& e)
								{
									std::cerr << e.what() << '\n';
								}
								
								break;
							}
						}
					}
				}
			ImGui::EndTable();
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::Button("+", ImVec2(32, window.SCREEN_HEIGHT - 52)))
		{
			m_schedule->addColumn(m_schedule->getColumnCount());
		}
		if (ImGui::Button("Add row", ImVec2(window.SCREEN_WIDTH - 58, 32)))
		{
			m_schedule->addRow(m_schedule->getRowCount());
		}
		ImGui::PopStyleVar();
    ImGui::End();
}

// Display the time editor popup's contents. The returned bool indicates whether any of the elements were edited.
bool ScheduleGui::displayTimeEditor(Time& value)
{
	bool valueEdited = false;

	bool tempDisplayTime = m_schedule->getColumnDisplayTime(m_timeEditorColumn);

	if (ImGui::Checkbox("##DisplayTime?", &tempDisplayTime))
	{
		m_schedule->setColumnDisplayTime(m_timeEditorColumn, tempDisplayTime);
	}
	ImGui::SameLine();
	char hourBuf[3];
	std::strftime(hourBuf, sizeof(hourBuf), "%H", value.getTime());
	ImGui::SetNextItemWidth(24);
	if (ImGui::InputText("##EditTimeHours", hourBuf, 3, ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll, filterNumbers))
	{
		int hourValue = 0;
		if (std::regex_match(hourBuf, std::regex("[0-9]+")))
		{
			hourValue = std::stoi(hourBuf);
		}
		value.setClockTime(hourValue, value.getTime()->tm_min);
		valueEdited = true;
	}
	ImGui::SameLine();
	char minBuf[3];
	std::strftime(minBuf, sizeof(minBuf), "%M", value.getTime());
	ImGui::SetNextItemWidth(24);
	if (ImGui::InputText("Time##Minutes", minBuf, 3, ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll, filterNumbers))
	{
		int minValue = 0;
		if (std::regex_match(minBuf, std::regex("[0-9]+")))
		{
			minValue = std::stoi(minBuf);
		}
		value.setClockTime(value.getTime()->tm_hour, minValue);
		valueEdited = true;
	}

	tm weekdayTime;
	bool selections[7];
	const bool* previousSelections = value.getSelectedWeekdays();
	std::copy(previousSelections, previousSelections + 7, std::begin(selections));

	bool tempDisplayWeekday = m_schedule->getColumnDisplayWeekday(m_timeEditorColumn);

	if (ImGui::Checkbox("##DisplayWeekday?", &tempDisplayWeekday))
	{
		m_schedule->setColumnDisplayWeekday(m_timeEditorColumn, tempDisplayWeekday);
	}
	ImGui::SameLine();
	for (int i = 0; i < 7; i++)
	{
		weekdayTime.tm_wday = i == 0 ? 1 : (i == 6 ? 0 : i + 1);
		char dayName[100];
		std::strftime(dayName, sizeof(dayName), "%a", &weekdayTime);

		if (ImGui::Selectable(dayName, &selections[i], ImGuiSelectableFlags_DontClosePopups, ImVec2(24, 0)))
		{
			value.setWeekdaySelected(i, selections[i]);
			valueEdited = true;
		}
		if (i != 6)
		{
			ImGui::SameLine();
		}
	}

	return valueEdited;
}

int ScheduleGui::filterNumbers(ImGuiInputTextCallbackData* data)
{
	if (data->EventChar > 47 && data->EventChar < 58)
		return 0;
	return 1;
}