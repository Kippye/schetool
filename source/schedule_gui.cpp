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
					ImGui::TableSetupColumn(m_schedule->getColumn(column)->name.c_str());
				}
				// custom header row
				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
				for (int column = 0; column < m_schedule->getColumnCount(); column++)
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

						SCHEDULE_TYPE columnType = m_schedule->getColumn(column)->type;
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
									ImGui::InputInt(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &value, 0, 100, ImGuiInputTextFlags_EnterReturnsTrue);
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
									ImGui::InputDouble(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &value, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue);
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
									if (ImGui::InputTextMultiline(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity(), ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
									{
										m_schedule->setElement<std::string>(column, row, std::string(buf));
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
							case(SCH_TIME):
							{
								try
								{
									auto value = m_schedule->getElement<Time>(column, row);
									if (m_schedule->getColumn(column)->displayWeekday)
									{
										const bool* selectedWeekdays = value.getSelectedWeekdays();
										unsigned int selectedWeekdayCount = 0;
										
										for (int i = 0; i < 7; i++)
										{
											if (selectedWeekdays[i] == true) { selectedWeekdayCount += 1; }
										}

										std::vector<std::string> displayedWeekdayNames = containers::split(value.getString(m_schedule->getColumn(column)->displayDate, m_schedule->getColumn(column)->displayTime, m_schedule->getColumn(column)->displayWeekday), ' ');
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
														m_timeEditorTime = tm(*value.getTime());
														m_timeEditorAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
														ImGui::OpenPopup("TimeEditor");
													}
													// right clicking erases the day - bonus feature
													else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
													{
														value.setWeekdaySelected(i, !selectedWeekdays[i]);
														m_schedule->setElement<Time>(column, row, value);
													}
												}
												ImGui::PopStyleColor(1);
												weekdayNameIndex++;
											}
										}
										// TEMP ? if there are no weekdays selected, just show an "Edit" button to prevent kind of a softlock
										if (selectedWeekdayCount == 0)
										{
											if (ImGui::Button(std::string("Edit##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
											{
												m_timeEditorColumn = column;
												m_timeEditorRow = row;
												m_timeEditorTime = tm(*value.getTime());
												m_timeEditorAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
												ImGui::OpenPopup("TimeEditor");
											}
										}
									}
									else
									{
										// Button displaying the date, time or selected weekdays of the current Time element
										if (ImGui::Button(value.getString(m_schedule->getColumn(column)->displayDate, m_schedule->getColumn(column)->displayTime, m_schedule->getColumn(column)->displayWeekday).append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
										{
											m_timeEditorColumn = column;
											m_timeEditorRow = row;
											m_timeEditorTime = tm(*value.getTime());
											m_timeEditorAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
											ImGui::OpenPopup("TimeEditor");
										}
									}
									if (column == m_timeEditorColumn && row == m_timeEditorRow && ImGui::BeginPopupEx(ImGui::GetID("TimeEditor"), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
									{
										// TODO!
										ImGuiDir dir = ImGuiDir_Down;
										ImGuiWindow* popup = ImGui::GetCurrentWindow();
										ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
										ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
										ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(popup->Pos, autoFitSize, &popup->AutoPosLastDirection, r_outer, m_timeEditorAvoidRect, ImGuiPopupPositionPolicy_Default));
										//return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, ImRect(window->Pos, window->Pos), ImGuiPopupPositionPolicy_Default); // Ideally we'd disable r_avoid here

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
			m_schedule->addDefaultColumn(m_schedule->getColumnCount());
		}
		if (ImGui::Button("Add row", ImVec2(window.SCREEN_WIDTH - 58, 32)))
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
bool ScheduleGui::displayTimeEditor(Time& value)
{
	bool valueEdited = false;
	bool isPermanentColumn = m_schedule->getColumn(m_timeEditorColumn)->permanent;
	// TIME
	bool tempDisplayTime = m_schedule->getColumn(m_timeEditorColumn)->displayTime;

	if (isPermanentColumn == false)
	{
		if (ImGui::Checkbox("##DisplayTime?", &tempDisplayTime))
		{
			m_schedule->setColumnDisplayTime(m_timeEditorColumn, tempDisplayTime);
		}
		ImGui::SameLine();
	}
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
	if (ImGui::InputText("##Minutes", minBuf, 3, ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll, filterNumbers))
	{
		int minValue = 0;
		if (std::regex_match(minBuf, std::regex("[0-9]+")))
		{
			minValue = std::stoi(minBuf);
		}
		value.setClockTime(value.getTime()->tm_hour, minValue);
		valueEdited = true;
	}

	// WEEKDAY
	tm weekdayTime;
	bool selections[7];
	const bool* previousSelections = value.getSelectedWeekdays();
	std::copy(previousSelections, previousSelections + 7, std::begin(selections));

	bool tempDisplayWeekday = m_schedule->getColumn(m_timeEditorColumn)->displayWeekday;

	if (isPermanentColumn == false)
	{
		if (ImGui::Checkbox("##DisplayWeekday?", &tempDisplayWeekday))
		{
			m_schedule->setColumnDisplayWeekday(m_timeEditorColumn, tempDisplayWeekday);
		}
		ImGui::SameLine();
	}
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

	// DATE / CALENDAR
	bool tempDisplayDate = m_schedule->getColumn(m_timeEditorColumn)->displayDate;

	if (isPermanentColumn == false)
	{
		if (ImGui::Checkbox("##DisplayDate?", &tempDisplayDate))
		{
			m_schedule->setColumnDisplayDate(m_timeEditorColumn, tempDisplayDate);
		}
		ImGui::SameLine();
	}

	if (ImGui::ArrowButton("##PreviousMonth", ImGuiDir_Left))
	{
		m_timeEditorTime.tm_mon = m_timeEditorTime.tm_mon == 0 ? 11 : m_timeEditorTime.tm_mon - 1;
	}
	ImGui::SameLine();
	char monthName[16];
	std::strftime(monthName, sizeof(monthName), "%B", &m_timeEditorTime);
	ImGui::Button(std::string(monthName).append(std::string("##Month")).c_str(), ImVec2(96, 0));
	ImGui::SameLine();
	if (ImGui::ArrowButton("##NextMonth", ImGuiDir_Right))
	{
		m_timeEditorTime.tm_mon = m_timeEditorTime.tm_mon == 11 ? 0 : m_timeEditorTime.tm_mon + 1;
	}
	int yearInput = m_timeEditorTime.tm_year + 1900;
	if (ImGui::InputInt("##YearInput", &yearInput, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		m_timeEditorTime.tm_year = Time::convertToValidYear(yearInput);
	}

	// TODO: actual calendar :(
	unsigned int daysInMonth = mytime::get_month_day_count(m_timeEditorTime);

	for (unsigned int day = 1; day <= daysInMonth; day++)
	{
		if (ImGui::Button(std::to_string(day).c_str(), ImVec2(24, 24)))
		{
			m_timeEditorTime.tm_mday = day;
			value.setYear(m_timeEditorTime.tm_year, false);
			value.setMonth(m_timeEditorTime.tm_mon);
			value.setMonthDay(m_timeEditorTime.tm_mday); 
			valueEdited = true;
		}
		if (day == value.getTime()->tm_mday)
		{
			// TODO: Highlight this day as selected in the calendar
		}
		if (day % 7 != 0)
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