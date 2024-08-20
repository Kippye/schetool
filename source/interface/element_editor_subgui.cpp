#include <regex>
#include "element_editor_subgui.h"
#include "gui_templates.h"
#include "schedule_constants.h"

ElementEditorSubGui::ElementEditorSubGui(const char* ID, const ScheduleCore& scheduleCore) : m_scheduleCore(scheduleCore), Gui(ID) 
{}

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
		
		switch(m_editedType)
		{
            case(SCH_TEXT):
            {
                if (gui_templates::TextEditor(m_editorText, m_textInputBoxSize, m_openLastFrame == false))
                {
                    m_madeEdits = true;
                    ImGui::CloseCurrentPopup();
                }
                break;
            }
			case(SCH_TIME):
			{
                if (gui_templates::TimeEditor(m_editorTime))
                {
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
				const std::vector<std::string>& optionNames = m_scheduleCore.getColumnSelectOptions(m_editorColumn).getOptions();

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
				if (m_scheduleCore.getColumnSelectOptions(m_editorColumn).getIsMutable())
				{
					std::string str;
					str.reserve(schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH);
					char* buf = str.data();
					//ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
					if (ImGui::InputText("##EditorOptionInput", buf, schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
					{
						if (std::string(buf).empty() == false)
						{
							modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_ADD)
								.firstIndex(0)
								.optionNames(std::vector<std::string>{std::string(buf)}));
							m_editorSelect.update(m_scheduleCore.getColumnSelectOptions(m_editorColumn).getLastChange(), m_scheduleCore.getColumnSelectOptions(m_editorColumn).getOptionCount());
							m_editorSelect.setSelected(m_scheduleCore.getColumnSelectOptions(m_editorColumn).getOptions().size() - 1, true);
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

					if (m_scheduleCore.getColumnSelectOptions(m_editorColumn).getIsMutable())
					{
						if (ImGui::SmallButton(std::string("X##").append(std::to_string(i)).c_str()))
						{
							modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_REMOVE).firstIndex(i));
							m_editorSelect.update(m_scheduleCore.getColumnSelectOptions(m_editorColumn).getLastChange(), m_scheduleCore.getColumnSelectOptions(m_editorColumn).getOptionCount());
							m_madeEdits = true;
							// break because the whole thing must be restarted now
							goto break_select_case;
						}
						ImGui::SameLine();
					}

					std::string optionButtonID = std::string(optionNames[i]).append("##EditorOption");
                    bool prevSelected = selected;

					if (ImGui::Selectable(optionButtonID.c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
					{
                        // Don't change option selection when drag is ended
                        if (m_draggedOptionID == optionButtonID && m_hasOptionBeenDragged)
                        {
                            selected = prevSelected;
                        }
                        else
                        {
                            m_editorSelect.setSelected(i, selected);
                            m_madeEdits = true;
                        }
					}

					// drag to reorder options
					if (m_scheduleCore.getColumnSelectOptions(m_editorColumn).getIsMutable())
					{
                        // ImGui::IsItemToggledSelection()
                        if (ImGui::IsItemActive())
                        {
                            if (optionButtonID != m_draggedOptionID)
                            {
                                m_hasOptionBeenDragged = false;
                                m_dragLastMousePos = ImGui::GetMousePos();
                                m_draggedOptionID = optionButtonID;
                            }
                            
                            if (!ImGui::IsItemHovered())
                            {
                                size_t i_next = i + ((ImGui::GetMousePos() - m_dragLastMousePos).y < 0.f ? -1 : 1);
                                if (i_next >= 0 && i_next < optionNames.size())
                                {
                                    m_hasOptionBeenDragged = true;
                                    modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_MOVE).firstIndex(i).secondIndex(i_next));
                                    m_editorSelect.update(m_scheduleCore.getColumnSelectOptions(m_editorColumn).getLastChange(), m_scheduleCore.getColumnSelectOptions(m_editorColumn).getOptionCount());
                                    m_madeEdits = true;
                                    m_dragLastMousePos = ImGui::GetMousePos();
                                }
                            }
                        }
                        // Drag ended
                        else if (m_draggedOptionID == optionButtonID)
                        {
                            m_draggedOptionID = "";
                            m_hasOptionBeenDragged = false;
                            m_dragLastMousePos = ImVec2(0, 0);
                        }
					}
				}

				break;
			}
            case(SCH_WEEKDAY):
			{
				auto selection = m_editorWeekday.getSelection();
				size_t selectedCount = selection.size();
				const std::vector<std::string>& optionNames = schedule_consts::weekdayNames;

				std::vector<size_t> selectionIndices = {};

				for (size_t s: selection)
				{
					selectionIndices.push_back(s);
				}

				// sort indices so that the same options are always displayed in the same order
				std::sort(std::begin(selectionIndices), std::end(selectionIndices));
				
				for (size_t i = 0; i < selectedCount; i++)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, gui_colors::dayColors[selectionIndices[i]]);
					if (ImGui::ButtonEx(std::string(optionNames[selectionIndices[i]]).append("##EditorSelectedOption").append(std::to_string(i)).c_str(), ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
					{
						m_editorWeekday.setSelected(selectionIndices[i], false);
						m_madeEdits = true;
					}
                    // no sameline for the last selected option
                    if (i != selectedCount - 1)
                    {
					    ImGui::SameLine();
                    }
					ImGui::PopStyleColor(1);
				}

				// display existing options
				for (size_t i = 0; i < optionNames.size(); i++)
				{
					bool selected = selection.find(i) != selection.end();

					std::string optionName = std::string(optionNames[i]);

                    ImGui::PushStyleColor(ImGuiCol_Header, gui_colors::dayColors[i]);
					if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
					{
						m_editorWeekday.setSelected(i, selected);
						m_madeEdits = true;
					}
                    ImGui::PopStyleColor(1);
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

void ElementEditorSubGui::setTextInputBoxSize(ImVec2 size)
{
    m_textInputBoxSize = size;
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