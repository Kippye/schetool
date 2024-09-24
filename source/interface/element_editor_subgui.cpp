#include <regex>
#include "element_editor_subgui.h"
#include "gui_templates.h"
#include "schedule_constants.h"
#include "gui_constants.h"

ElementEditorSubGui::ElementEditorSubGui(const char* ID, const ScheduleCore& scheduleCore) : m_scheduleCore(scheduleCore), Gui(ID) 
{}

void ElementEditorSubGui::draw(Window& window, Input& input, GuiTextures& guiTextures)
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
				const std::vector<SelectOption>& options = m_scheduleCore.getColumnSelectOptions(m_editorColumn).getOptions();

				std::vector<size_t> selectionIndices = {};

				for (size_t s: selection)
				{
					selectionIndices.push_back(s);
				}

				// sort indices so that the same options are always displayed in the same order
				std::sort(std::begin(selectionIndices), std::end(selectionIndices));
				
				for (size_t i = 0; i < selectedCount; i++)
				{
					if (gui_templates::SelectOptionButton(options[selectionIndices[i]], "##EditorSelectedOption", ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
					{
						m_editorSelect.setSelected(selectionIndices[i], false);
						m_madeEdits = true;
					}
					ImGui::SameLine();
				}

				// add new options
				if (m_scheduleCore.getColumnSelectOptions(m_editorColumn).getIsMutable())
				{
                    const SelectOptions& selectOptions = m_scheduleCore.getColumnSelectOptions(m_editorColumn);

					std::string str;
					str.reserve(schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH);
					char* buf = str.data();
                    ImGui::SetNextItemWidth(gui_sizes::selectOptionSelectableWidth); // Reuse this size for the input as well.
                    if (m_openLastFrame == false) // Give the option name input keyboard focus if the editor was just opened
                    {
                        ImGui::SetKeyboardFocusHere();
                    }
					if (ImGui::InputText("##EditorOptionInput", buf, schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						if (std::string(buf).empty() == false)
						{
                            SelectColor addedOptionColor = SelectColor_White;
                            // If there is a previous select option, get the "next" color from its color.
                            if (selectOptions.getOptionCount() > 0)
                            {
                                SelectColor lastOptionColor = selectOptions.getOptions().back().color;
                                // Add 1 to the last color if it's 0, otherwise multiply by 2 or loop around to 0 if needed.
                                addedOptionColor = lastOptionColor == 0 ? 1 : (lastOptionColor * 2 < SelectColor_Last ? lastOptionColor * 2 : 0);
                            } 
							modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_ADD)
								.options({SelectOption(std::string(buf), addedOptionColor)}));
							// HACK: There's currently no way of knowing that the option was successfully added.
                            // We just check the things that we can and if they are true, assume that it did succeed.
                            if ((selectOptions.getLastUpdateInfo().firstIndex == selectOptions.getOptionCount() - 1 && selectOptions.getLastUpdateInfo().type == OPTION_MODIFICATION_ADD)
                                && selectOptions.getOptions().back().name == buf)
                            {
                                m_editorSelect.update(selectOptions.getLastUpdateInfo(), selectOptions.getOptionCount());
                                m_editorSelect.setSelected(selectOptions.getOptions().size() - 1, true);
                                m_madeEdits = true;
                                // NOTE: break here because otherwise the start and end of the function kind of go out of sync
                                break;
                            }
						}
					}
				}
                // if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
                // OK i give up doing this for now
                // Maybe future me can finger this out.. TODO!
                // if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !m_giveSelectOptionNameInputFocus)
                // {
                //     printf("Regave focus!\n");
                //     ImGui::ActivateItemByID(ImGui::GetItemID());
                // }

				// display existing options
				for (size_t i = 0; i < options.size(); i++)
				{
					bool selected = selection.find(i) != selection.end();

                    std::string optionButtonID = std::string(options[i].name).append("##EditorOption");

                    // Draw a name input in place of the usual selectable
                    if (m_editingSelectOptionName && m_editedOptionIndex == i)
                    {
                        std::string name = options[i].name;
                        name.reserve(schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH);
                        char* buf = name.data();

                        if (m_giveSelectOptionNameInputFocus)
                        {
                            ImGui::SetKeyboardFocusHere(0);
                            m_giveSelectOptionNameInputFocus = false;
                        }

                        ImGui::SetNextItemWidth(gui_sizes::selectOptionSelectableWidth);
                        if (ImGui::InputText("##SelectOptionNameInput", buf, schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue, gui_callbacks::filterAlphanumerics))
                        {
                            modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_RENAME).firstIndex(m_editedOptionIndex).name(buf));
                            m_editingSelectOptionName = false;
                        }

                        if (ImGui::IsItemDeactivated())
                        {
                            m_editingSelectOptionName = false;
                        }
                    }
                    // Draw the selectable for this select option
                    else
                    {
                        bool prevSelected = selected;
                        ImGui::SetNextItemAllowOverlap();
                        if (gui_templates::SelectOptionSelectable(options[i], "##EditorOption", &selected, ImVec2(gui_sizes::selectOptionSelectableWidth, 0)))
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
                        const float optionButtonHeight = ImGui::GetItemRectSize().y;
                        const float optionButtonRectMaxX = ImGui::GetItemRectMax().x;

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            // Since i can't make double clicking do nothing (without some complicated stuffery), the option just gets its selection set again on the second click.
                            selected = !selected;
                            m_editorSelect.setSelected(i, selected);
                            // Start editing this select option's name
                            m_editingSelectOptionName = true;
                            m_giveSelectOptionNameInputFocus = true;
                            m_editedOptionIndex = i;
                        }
                        // Show a remove button on the right when hovered, only if the options are mutable
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && m_scheduleCore.getColumnSelectOptions(m_editorColumn).getIsMutable())
                        {
                            float removeButtonSize = optionButtonHeight; //ImGui::GetFrameHeight();
                            ImGui::SameLine();
                            float previousCursorPosX = ImGui::GetCursorPosX();
                            ImGui::SetCursorScreenPos(ImVec2(optionButtonRectMaxX - removeButtonSize, ImGui::GetCursorScreenPos().y));
                            size_t pushedColorCount = 0;
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); pushedColorCount++;
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f)); pushedColorCount++;
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.4f)); pushedColorCount++;
                            if (ImGui::ImageButtonEx(ImGui::GetID(std::format("##RemoveSelectOption{}", i).c_str()), (ImTextureID)guiTextures.getOrLoad("icon_remove"), ImVec2(removeButtonSize, removeButtonSize) - ImGui::GetStyle().FramePadding * 2.0f, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f)))
                            {
                                modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_REMOVE).firstIndex(i));
                                m_editorSelect.update(m_scheduleCore.getColumnSelectOptions(m_editorColumn).getLastUpdateInfo(), m_scheduleCore.getColumnSelectOptions(m_editorColumn).getOptionCount());
                                m_madeEdits = true;
                                ImGui::PopStyleColor(pushedColorCount);
                                // break because the whole thing must be restarted now
                                goto break_select_case;
                            }
                            ImGui::PopStyleColor(pushedColorCount);
                        }
                    }

					// drag to reorder options
					if (m_editingSelectOptionName == false && m_scheduleCore.getColumnSelectOptions(m_editorColumn).getIsMutable())
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
                                if (i_next >= 0 && i_next < options.size())
                                {
                                    m_hasOptionBeenDragged = true;
                                    modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_MOVE).firstIndex(i).secondIndex(i_next));
                                    m_editorSelect.update(m_scheduleCore.getColumnSelectOptions(m_editorColumn).getLastUpdateInfo(), m_scheduleCore.getColumnSelectOptions(m_editorColumn).getOptionCount());
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

                    ImGui::SameLine();
                    // Edit name button
                    // if (ImGui::ImageButton(std::format("EditSelectOptionName{}", i).c_str(), (ImTextureID)guiTextures.getOrLoad("icon_edit"), gui_sizes::element_editor::selectOptionEditButtonSize - ImGui::GetStyle().FramePadding * 2.0f))
                    // {
                    //     m_editingSelectOptionName = true;
                    //     m_giveSelectOptionNameInputFocus = true;
                    //     m_editedOptionIndex = i;
                    // }
                    // ImGui::SameLine();
                    // Edit color button
                    if (ImGui::ColorButton(std::format("##EditSelectOptionColor{}", i).c_str(), gui_color_calculations::hslToRgb(gui_colors::selectOptionColors.at(options[i].color)), ImGuiColorEditFlags_NoTooltip))
                    {
                        m_colorChooserOptionIndex = i;
                        ImGui::OpenPopup("SelectOptionColorChooserPopup");
                    }
                    ImRect colorChooserButtonAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                    // Color chooser popup, shows a list of available select option colors.
                    if (m_colorChooserOptionIndex == i && ImGui::BeginPopup("SelectOptionColorChooserPopup"))
                    {
                        ImGuiWindow* popup = ImGui::GetCurrentWindow();
                        ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
                        ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
                        ImVec2 popupPos = ImVec2(popup->Pos.x, colorChooserButtonAvoidRect.Min.y - ImGui::GetStyle().WindowPadding.y);
                        ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(popupPos, autoFitSize, &popup->AutoPosLastDirection, r_outer, colorChooserButtonAvoidRect, ImGuiPopupPositionPolicy_Default));

                        // Draw buttons for all possible select option colors
                        for (auto [colorEnum, colorHsl] : gui_colors::selectOptionColors)
                        {
                            if (ImGui::ColorButton(std::format("##ColorChooserPopupColor{}", colorEnum).c_str(), gui_color_calculations::hslToRgb(colorHsl), ImGuiColorEditFlags_NoTooltip))
                            {
                                modifyColumnSelectOptions.invoke(m_editorColumn, SelectOptionsModification(OPTION_MODIFICATION_RECOLOR).firstIndex(i).color(colorEnum));
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::EndPopup();
                    }
				}

				break;
			}
            case(SCH_WEEKDAY):
			{
				auto selection = m_editorWeekday.getSelection();
				size_t selectedCount = selection.size();
				const std::vector<std::string>& optionNames = general_consts::weekdayNames;

				std::vector<size_t> selectionIndices = {};

				for (size_t s: selection)
				{
					selectionIndices.push_back(s);
				}

				// sort indices so that the same options are always displayed in the same order
				std::sort(std::begin(selectionIndices), std::end(selectionIndices));
				
				for (size_t i = 0; i < selectedCount; i++)
				{
					if (gui_templates::SelectOptionButton(SelectOption{optionNames[selectionIndices[i]], gui_colors::dayColors[selectionIndices[i]]}, std::format("##EditorSelectedOption{}", i).c_str(), ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
					{
						m_editorWeekday.setSelected(selectionIndices[i], false);
						m_madeEdits = true;
					}
                    // no sameline for the last selected option
                    if (i != selectedCount - 1)
                    {
					    ImGui::SameLine();
                    }
				}

				// display existing options
				for (size_t i = 0; i < optionNames.size(); i++)
				{
					bool selected = selection.find(i) != selection.end();

                    if (gui_templates::SelectOptionSelectable(SelectOption{optionNames[i], gui_colors::dayColors[i]}, std::format("##EditorOption{}", i).c_str(), &selected, ImVec2(gui_sizes::selectOptionSelectableWidth, 0), ImGuiSelectableFlags_DontClosePopups))
					{
						m_editorWeekday.setSelected(i, selected);
						m_madeEdits = true;
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
    m_editingSelectOptionName = false;

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