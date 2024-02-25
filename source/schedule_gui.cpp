#include <schedule_gui.h>
#include <schedule.h>
#include <imgui.h>
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
		ImGui::BeginChild("SchedulePanel", ImVec2(0, window.SCREEN_HEIGHT - 52), true);
			// TODO: the schedule module tells THIS what to display?
			ImGui::BeginTable("ScheduleTable", m_schedule->getColumnCount(), ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableRowFlags_Headers | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ContextMenuInBody);
				for (int column = 0; column < m_schedule->getColumnCount(); column++)
				{
					ImGui::TableSetupColumn(m_schedule->getColumnName(column));
				}
				ImGui::TableHeadersRow();

				for (int row = 0; row < m_schedule->getRowCount(); row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < m_schedule->getColumnCount(); column++)
					{
						ImGui::TableSetColumnIndex(column);
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
									ImGui::Text(std::to_string(value).c_str());
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
									ImGui::Text(std::to_string(value).c_str());
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
									auto value = m_schedule->getElement<std::string>(column, row);
									ImGui::Text(value.c_str());
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
		if (ImGui::Button("Add row", ImVec2(ImGui::GetWindowWidth(), 32.0f)))
		{
			m_schedule->addRow(m_schedule->getRowCount());
		}
		ImGui::PopStyleVar();
    ImGui::End();
}