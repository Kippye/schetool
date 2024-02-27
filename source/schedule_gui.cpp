#include <schedule_gui.h>
#include <schedule.h>
#include <imgui.h>
#include <string>
#include <window.h>
#include <any>
#include <cstdio>

#include <iostream>
#include <time_container.h>

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
									ImGui::Text(value.getString().c_str());
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