#include <regex>
#include "element_editor_subgui.h"
#include "gui_templates.h"

ElementEditorSubGui::ElementEditorSubGui(const char* ID, const ScheduleCore* scheduleCore) : Gui(ID) 
{
	m_scheduleCore = scheduleCore;
}

void ElementEditorSubGui::draw(Window& window, Input& input)
{
	// give old current open state to the last frame's state
	m_openLastFrame = m_openThisFrame;

	if (ImGui::BeginPopupEx(ImGui::GetID("Editor"), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
	{
		// TODO clean up / make the positioning more precise!
		ImGuiDir dir = ImGuiDir_Down;
		ImGuiWindow* popup = ImGui::GetCurrentWindow();
		ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
		ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
		ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(popup->Pos, autoFitSize, &popup->AutoPosLastDirection, r_outer, m_avoidRect, ImGuiPopupPositionPolicy_Default));
		//return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, ImRect(window->Pos, window->Pos), ImGuiPopupPositionPolicy_Default); // Ideally we'd disable r_avoid here

		bool isPermanentColumn = m_scheduleCore->getColumn(m_editorColumn)->permanent;
		tm formatTime;
		
		switch(m_editedType)
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
					m_madeEdits = true;
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
					m_madeEdits = true;
				}

				break;
			}
			case(SCH_DATE):
			{
				if (gui_templates::DateEditor(m_editorDate, m_viewedYear, m_viewedMonth))
                {
                    m_madeEdits = true;
                }

				break;
			}
			case(SCH_SELECT):
			{
				auto selection = m_editorSelect.getSelection();
				size_t selectedCount = selection.size();
				const std::vector<std::string>& optionNames = m_scheduleCore->getColumnSelectOptions(m_editorColumn).getOptions();

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
						m_madeEdits = true;
					}
					ImGui::SameLine();
					// ImGui::PopStyleColor(1);
				}

				// add new options
				if (m_scheduleCore->getColumnSelectOptions(m_editorColumn).getIsMutable() && optionNames.size() < SELECT_OPTION_COUNT_MAX)
				{
					std::string str;
					str.reserve(SELECT_OPTION_NAME_MAX_LENGTH);
					char* buf = str.data();
					//ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
					if (ImGui::InputText("##EditorOptionInput", buf, SELECT_OPTION_NAME_MAX_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
					{
						if (std::string(buf).empty() == false)
						{
							modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_ADD)
								.firstIndex(0)
								.optionNames(std::vector<std::string>{std::string(buf)}));
							m_editorSelect.update(m_scheduleCore->getColumnSelectOptions(m_editorColumn).getLastChange(), m_scheduleCore->getColumnSelectOptions(m_editorColumn).getOptionCount());
							m_editorSelect.setSelected(m_scheduleCore->getColumnSelectOptions(m_editorColumn).getOptions().size() - 1, true);
							m_madeEdits = true;
							// NOTE: break here because otherwise the start and end of the function kind of go out of sync
							break;
						}
					}
				}

				// display existing options
				for (size_t i = 0; i < optionNames.size(); i++)
				{
					bool selected = selection.find(i) != selection.end();

					if (m_scheduleCore->getColumnSelectOptions(m_editorColumn).getIsMutable())
					{
						if (ImGui::SmallButton(std::string("X##").append(std::to_string(i)).c_str()))
						{
							modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_REMOVE).firstIndex(i));
							m_editorSelect.update(m_scheduleCore->getColumnSelectOptions(m_editorColumn).getLastChange(), m_scheduleCore->getColumnSelectOptions(m_editorColumn).getOptionCount());
							m_madeEdits = true;
							// break because the whole thing must be restarted now
							goto break_select_case;
						}
						ImGui::SameLine();
					}

					std::string optionName = std::string(optionNames[i]);

					if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
					{
						m_editorSelect.setSelected(i, selected);
						m_madeEdits = true;
					}

					// drag to reorder options
					if (m_scheduleCore->getColumnSelectOptions(m_editorColumn).getIsMutable())
					{
						if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
						{
							size_t i_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
							if (i_next >= 0 && i_next < optionNames.size())
							{
								modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_MOVE).firstIndex(i).secondIndex(i_next));
								m_editorSelect.update(m_scheduleCore->getColumnSelectOptions(m_editorColumn).getLastChange(), m_scheduleCore->getColumnSelectOptions(m_editorColumn).getOptionCount());
								m_madeEdits = true;
								ImGui::ResetMouseDragDelta();
								break;
							}
						}
					}
				}

				break;
			}
			default:
            {
                ImGui::EndPopup();
                return;
            }
		}
		break_select_case:

		m_openThisFrame = true;
		ImGui::EndPopup();
	}
	else
	{
		m_openThisFrame = false;
	}
}

void ElementEditorSubGui::open(size_t column, size_t row, SCHEDULE_TYPE type, const ImRect& avoidRect)
{
	m_editorColumn = column;
	m_editorRow = row;
	m_editedType = type;
	m_avoidRect = avoidRect;

	m_madeEdits = false;

	ImGui::OpenPopup("Editor");
}

bool ElementEditorSubGui::getOpenThisFrame() const
{
	return  m_openThisFrame;
}

bool ElementEditorSubGui::getOpenLastFrame() const
{
	return  m_openLastFrame;
}

bool ElementEditorSubGui::getMadeEdits() const
{
	return m_madeEdits;
}

std::pair<size_t, size_t> ElementEditorSubGui::getCoordinates() const
{
	return {m_editorColumn, m_editorRow};
}

int ElementEditorSubGui::filterNumbers(ImGuiInputTextCallbackData* data)
{
	if (data->EventChar > 47 && data->EventChar < 58)
		return 0;
	return 1;
}