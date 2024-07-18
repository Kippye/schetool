#include "filter_editor_subgui.h"
#include "gui_templates.h"
#include "schedule_constants.h"

using filter_consts::TypeComparisonInfo;

void FilterGroupEditorState::setup(SCHEDULE_TYPE type, FilterGroup filterGroup)
{
    m_type = type;
    m_filterGroup = filterGroup;
    m_isValid = true;
}

SCHEDULE_TYPE FilterGroupEditorState::getType() const
{
    return m_type;
}

FilterGroup& FilterGroupEditorState::getFilterGroup()
{
    return m_filterGroup;
}

bool FilterGroupEditorState::getIsValid() const
{
    return m_isValid;
}


SCHEDULE_TYPE FilterRuleEditorState::getType() const
{
    return m_type;
}

std::pair<size_t, size_t> FilterRuleEditorState::getIndices() const
{
    return {m_filterIndex, m_filterRuleIndex};
}

FilterRuleContainer& FilterRuleEditorState::getFilterRule()
{
    return m_filterRule;
}
 
bool FilterRuleEditorState::getIsValid() const
{
    return m_isValid;
}


FilterEditorSubGui::FilterEditorSubGui(const char* ID, const ScheduleCore* scheduleCore, ScheduleEvents& scheduleEvents) : Gui(ID) 
{
	m_scheduleCore = scheduleCore;
    scheduleEvents.columnAdded.addListener(columnAddedListener);
    scheduleEvents.columnRemoved.addListener(columnRemovedListener);
}

void FilterEditorSubGui::draw(Window& window, Input& input)
{
	if (ImGui::BeginPopupEx(ImGui::GetID("FilterGroup Editor"), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
	{
        if (m_scheduleCore->existsColumnAtIndex(m_editorColumn) == false)
        {
            printf("FilterEditorSubGui::draw(): There is no Column at the editor column index %zu\n", m_editorColumn);
            ImGui::EndPopup();
            close();
            return;
        }
        if (m_scheduleCore->getColumn(m_editorColumn)->type != m_filterGroupState.getType())
        {
            printf("FilterEditorSubGui::draw(): The types of the Column (%d) and the editor's filter state (%d) do not match!\n", m_scheduleCore->getColumn(m_editorColumn)->type, m_filterGroupState.getType());
            ImGui::EndPopup();
            close();
            return;
        }
        if (m_filterGroupState.getIsValid() == false)
        {
            printf("FilterEditorSubGui::draw(): FilterGroupEditorState is not valid.\n");
            ImGui::EndPopup();
            close();
            return;
        }

		// TO/DO clean up / make the positioning more precise!
		// ImGuiDir dir = ImGuiDir_Down;
		// ImGuiWindow* popup = ImGui::GetCurrentWindow();
		// ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
		// ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
		//ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(popup->Pos, autoFitSize, &popup->AutoPosLastDirection, r_outer, m_avoidRect, ImGuiPopupPositionPolicy_Default));
		//return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, ImRect(window->Pos, window->Pos), ImGuiPopupPositionPolicy_Default); // Ideally we'd disable r_avoid here

		bool isPermanentColumn = m_scheduleCore->getColumn(m_editorColumn)->permanent;
		tm formatTime;

        // TODO: Reuse for opening FilterRules
        // Set up m_filterRuleState and open the popup
        // switch(columnType)
        // {
        //     case(SCH_BOOL):
        //     {
        //         m_filterGroupState.setFilter(std::make_shared<FilterRule<bool>>(false));
        //         break;
        //     }
        //     case(SCH_NUMBER):
        //     {
        //         m_filterGroupState.setFilter(std::make_shared<FilterRule<int>>(0));
        //         break;
        //     }
        //     case(SCH_DECIMAL):
        //     {
        //         m_filterGroupState.setFilter(std::make_shared<FilterRule<double>>(0.0));
        //         break;
        //     }
        //     case(SCH_TEXT):
        //     {
        //         m_filterGroupState.setFilter(std::make_shared<FilterRule<std::string>>(std::string("")));
        //         break;
        //     }
        //     case(SCH_SELECT):
        //     {
        //         m_filterGroupState.setFilter(std::make_shared<FilterRule<SelectContainer>>(SelectContainer()));
        //         break;
        //     }
        //     case(SCH_WEEKDAY):
        //     {
        //         m_filterGroupState.setFilter(std::make_shared<FilterRule<WeekdayContainer>>(WeekdayContainer()));
        //         break;
        //     }
        //     case(SCH_TIME):
        //     {
        //         m_filterGroupState.setFilter(std::make_shared<FilterRule<TimeContainer>>(TimeContainer(0, 0)));
        //         break;
        //     }
        //     case(SCH_DATE):
        //     {
        //         // default date mode to relative
        //         m_filterGroupState.setFilter(std::make_shared<FilterRule<DateContainer>>(DateContainer::getCurrentSystemDate()));
        //         m_filterGroupState.getFilter<DateContainer>()->setComparison(Comparison::IsRelativeToToday);
        //         break;
        //     }
        //     default:
        //     {
        //         printf("FilterEditorSubGui::open_create(%zu): Creating filters for type %d has not been implemented\n", column, columnType);
        //         return;
        //     }
        // }

        // editing and a remove button hasn't been added before
        if (m_editing == true)
        {
            ImGui::SameLine();
            // displayRemoveFilterButton();
        }

        if (m_editing == false && ImGui::Button("Create"))
        {
            addColumnFilterGroup.invoke(m_editorColumn, m_filterGroupState.getFilterGroup());
            ImGui::CloseCurrentPopup();
        }

		ImGui::EndPopup();
	}
}

void FilterEditorSubGui::drawRuleEditor()
{
    if (ImGui::BeginPopupEx(ImGui::GetID("FilterRule Editor"), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
	{
        if (m_filterRuleState.getIsValid() == false)
        {
            printf("FilterEditorSubGui::drawRuleEditor(): FilterRuleEditorState is not valid.\n");
            ImGui::EndPopup();
            close();
            return;
        }

		// TO/DO clean up / make the positioning more precise!
		// ImGuiDir dir = ImGuiDir_Down;
		// ImGuiWindow* popup = ImGui::GetCurrentWindow();
		// ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
		// ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
		//ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(popup->Pos, autoFitSize, &popup->AutoPosLastDirection, r_outer, m_avoidRect, ImGuiPopupPositionPolicy_Default));
		//return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, ImRect(window->Pos, window->Pos), ImGuiPopupPositionPolicy_Default); // Ideally we'd disable r_avoid here

		bool isPermanentColumn = m_scheduleCore->getColumn(m_editorColumn)->permanent;
		tm formatTime;

        // LAMBDA.
        // Display a Selectable combo for choosing the comparison with options for the provided type.
        // Returns a pair containing a bool (whether the comparison was changed) and a Comparison (the selected Comparison)
        auto displayComparisonCombo = [&](SCHEDULE_TYPE type) -> std::pair<bool, Comparison>
        {
            Comparison currentComparison = m_filterRuleState.getFilterRule().getComparison();
            TypeComparisonInfo comparisonInfo = filter_consts::getComparisonInfo(type);
            bool selectionChanged = false;

            bool hasMultipleOptions = comparisonInfo.names.size() > 1;
            if (hasMultipleOptions == false)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            }
            if (ImGui::BeginCombo("##filterEditorComparison", filter_consts::comparisonStrings.at(currentComparison)))
            {
                for (size_t i = 0; i < comparisonInfo.names.size(); i++)
                {
                    bool isSelected = comparisonInfo.comparisons[i] == currentComparison;
                    if (ImGui::Selectable(comparisonInfo.names[i].c_str(), isSelected))
                    {
                        m_filterRuleState.getFilterRule().setComparison(comparisonInfo.comparisons.at(i));
                        selectionChanged = true;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    // if (isSelected)
                    // {
                    //     ImGui::SetItemDefaultFocus();
                    // }
                }

                ImGui::EndCombo();
            }
            if (hasMultipleOptions == false)
            {
                ImGui::PopItemFlag();
            }
            return { selectionChanged, m_filterRuleState.getFilterRule().getComparison() };
        };

        bool hasRemoveButton = false;
    
        // LAMBDA
        // auto displayRemoveFilterButton = [&]() 
        // {
        //     if (ImGui::SmallButton("X##removeFilterRule"))
        //     {
        //         removeColumnFilter.invoke(m_editorColumn, m_editorFilterIndex);
        //         ImGui::CloseCurrentPopup();
        //     }
        //     hasRemoveButton = true;
        // };

        gui_templates::TextWithBackground("%s", m_scheduleCore->getColumn(m_editorColumn)->name.c_str());

        ImGui::SameLine();

		switch(m_filterGroupState.getType())
		{
            case(SCH_BOOL):
            {
                displayComparisonCombo(SCH_BOOL);

                ImGui::SameLine();

                bool newValue = m_filterRuleState.getFilterRule().getPassValue<bool>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<bool>(); 

                if (ImGui::Checkbox(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterRuleState.getFilterRule().setPassValue(newValue);
                    auto [filterIndex, filterRuleIndex] = m_filterRuleState.getIndices();
                    invokeFilterEditEvent<bool>(filterIndex, filterRuleIndex, prevFilter, m_filterRuleState.getFilterRule().getAsType<bool>());
                }
                break;
            }
            case(SCH_NUMBER):
            {
                displayComparisonCombo(SCH_NUMBER);

                ImGui::SameLine();

                int newValue = m_filterRuleState.getFilterRule().getPassValue<int>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<int>(); 

                if (ImGui::InputInt(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    m_filterRuleState.getFilterRule().setPassValue(newValue);
                    auto [filterIndex, filterRuleIndex] = m_filterRuleState.getIndices();
                    invokeFilterEditEvent<int>(filterIndex, filterRuleIndex, prevFilter, m_filterRuleState.getFilterRule().getAsType<int>());
                }
                break;
            }
            // case(SCH_DECIMAL):
            // {
            //     displayComparisonCombo(SCH_DECIMAL);

            //     ImGui::SameLine();

            //     double newValue = m_filterRuleState.getFilterRule().getAsType<double>()->getPassValue();
            //     auto prevFilter = *m_filterRuleState.getFilterRule().getAsType<double>(); 
                
            //     if (ImGui::InputDouble(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue, 0.0, 0.0, "%.15g", ImGuiInputTextFlags_EnterReturnsTrue))
            //     {
            //         m_filterRuleState.getFilterRule().getAsType<double>()->setPassValue(newValue);
            //         invokeFilterEditEvent<double>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<double>());
            //     }
            //     break;
            // }
            // case(SCH_TEXT):
            // {
            //     displayComparisonCombo(SCH_TEXT);

            //     if (m_editing == true)
            //     {
            //         ImGui::SameLine();
            //         displayRemoveFilterButton();
            //     }

            //     std::string value = m_filterRuleState.getFilterRule().getAsType<std::string>()->getPassValue();
            //     auto prevFilter = *m_filterRuleState.getFilterRule().getAsType<std::string>(); 
            //     value.reserve(schedule_consts::ELEMENT_TEXT_MAX_LENGTH);
            //     char* buf = value.data();
            //     //ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
            //     if (ImGui::InputTextMultiline(std::string("##filterEditor").append(std::to_string(m_editorColumn)).c_str(), buf, value.capacity(), ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
            //     {
            //         m_filterRuleState.getFilterRule().getAsType<std::string>()->setPassValue(std::string(buf));
            //         invokeFilterEditEvent<std::string>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<std::string>());
            //     }

            //     break;
            // }
            // case(SCH_SELECT):
            // {
			// 	SelectContainer value = m_filterRuleState.getFilterRule().getAsType<SelectContainer>()->getPassValue();
            //     auto prevFilter = *m_filterRuleState.getFilterRule().getAsType<SelectContainer>(); 

            //     displayComparisonCombo(SCH_SELECT);

            //     // Remove button
            //     if (m_editing == true)
            //     {
            //         ImGui::SameLine();
            //         displayRemoveFilterButton();
            //     }

            //     auto selection = value.getSelection();
			// 	const std::vector<std::string>& optionNames = m_scheduleCore->getColumnSelectOptions(m_editorColumn).getOptions();

            //     // Options
			// 	for (size_t i = 0; i < optionNames.size(); i++)
			// 	{
			// 		bool selected = selection.find(i) != selection.end();

            //         if (ImGui::Checkbox(std::string("##SelectFilterEditorCheck").append(std::to_string(i)).c_str(), &selected))
            //         {
            //             value.setSelected(i, selected);
            //             m_filterRuleState.getFilterRule().getAsType<SelectContainer>()->setPassValue(value);
            //             invokeFilterEditEvent<SelectContainer>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<SelectContainer>());
            //         }

            //         ImGui::SameLine();

			// 		std::string optionName = std::string(optionNames[i]);

			// 		if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
			// 		{
			// 			value.setSelected(i, selected);
            //             m_filterRuleState.getFilterRule().getAsType<SelectContainer>()->setPassValue(value);
            //             invokeFilterEditEvent<SelectContainer>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<SelectContainer>());
			// 		}
			// 	}
            //     break;
            // }
            // case(SCH_WEEKDAY):
            // {
			// 	WeekdayContainer value = m_filterRuleState.getFilterRule().getAsType<WeekdayContainer>()->getPassValue();
            //     auto prevFilter = *m_filterRuleState.getFilterRule().getAsType<WeekdayContainer>(); 

            //     displayComparisonCombo(SCH_WEEKDAY);

            //     // Remove button
            //     if (m_editing == true)
            //     {
            //         ImGui::SameLine();
            //         displayRemoveFilterButton();
            //     }

            //     if (m_filterRuleState.getFilterBase()->getComparison() != Comparison::ContainsToday)
            //     {
            //         auto selection = value.getSelection();
            //         const std::vector<std::string>& optionNames = schedule_consts::weekdayNames;
            //         // Options
            //         for (size_t i = 0; i < optionNames.size(); i++)
            //         {
            //             bool selected = selection.find(i) != selection.end();

            //             if (ImGui::Checkbox(std::string("##WeekdayFilterEditorCheck").append(std::to_string(i)).c_str(), &selected))
            //             {
            //                 value.setSelected(i, selected);
            //                 m_filterRuleState.getFilterRule().getAsType<WeekdayContainer>()->setPassValue(value);
            //                 invokeFilterEditEvent<WeekdayContainer>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<WeekdayContainer>());
            //             }

            //             ImGui::SameLine();

            //             std::string optionName = std::string(optionNames[i]);

            //             ImGui::PushStyleColor(ImGuiCol_Header, gui_colors::dayColors[i]);
            //             if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
            //             {
            //                 value.setSelected(i, selected);
            //                 m_filterRuleState.getFilterRule().getAsType<WeekdayContainer>()->setPassValue(value);
            //                 invokeFilterEditEvent<WeekdayContainer>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<WeekdayContainer>());
            //             }
            //             ImGui::PopStyleColor(1);
            //         }
            //     }
            //     break;
            // }
            // case(SCH_TIME):
            // {
            //     TimeContainer value = m_filterRuleState.getFilterRule().getAsType<TimeContainer>()->getPassValue();
            //     auto prevFilter = *m_filterRuleState.getFilterRule().getAsType<TimeContainer>(); 
                
            //     displayComparisonCombo(SCH_TIME);

            //     ImGui::SameLine();

            //     if (gui_templates::TimeEditor(value))
            //     {
            //         m_filterRuleState.getFilterRule().getAsType<TimeContainer>()->setPassValue(value);
            //         invokeFilterEditEvent<TimeContainer>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<TimeContainer>());
            //     }
 
            //     break;
            // }
            // case(SCH_DATE):
            // {
            //     DateContainer value = m_filterRuleState.getFilterRule().getAsType<DateContainer>()->getPassValue();
            //     auto prevFilter = *m_filterRuleState.getFilterRule().getAsType<DateContainer>(); 

            //     auto [_comparisonChanged, newComparison] = displayComparisonCombo(SCH_DATE);

            //     ImGui::SameLine();

            //     // !!! update value, might have been modified above
            //     value = m_filterRuleState.getFilterRule().getAsType<DateContainer>()->getPassValue();

            //     // Display the date of the current Date element, UNLESS the comparison is IsEmpty
            //     if (newComparison != Comparison::IsEmpty)
            //     {
            //         gui_templates::TextWithBackground("%s##filterEditor%zu", newComparison == Comparison::IsRelativeToToday ? "Today" : value.getString().c_str(), m_editorColumn);
            //     }
            //     else
            //     {
            //         ImGui::NewLine();
            //     }

            //     // If editing, display the remove filter button here, before the Date editor 
            //     if (m_editing == true)
            //     {
            //         ImGui::SameLine();
            //         displayRemoveFilterButton();
            //     }

            //     if (gui_templates::DateEditor(value, m_viewedYear, m_viewedMonth, false, false))
            //     {
            //         m_filterRuleState.getFilterRule().getAsType<DateContainer>()->setComparison(Comparison::Is);
            //         // weird lil thing: if a Date was selected, then the Date is no longer relative. so we make a copy using its time but with relative = false
            //         m_filterRuleState.getFilterRule().getAsType<DateContainer>()->setPassValue(DateContainer(value.getTime()));
            //         invokeFilterEditEvent<DateContainer>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<DateContainer>());
            //     }
                
            //     break;
            // }
			default:
            {
                ImGui::EndPopup();
                return;
            }
		}

        // editing and a remove button hasn't been added before
        if (m_editing == true && hasRemoveButton == false)
        {
            // NOTE: dunno if this will have the remove button anymore
            // ImGui::SameLine();
            // displayRemoveFilterButton();
        }

        // TODO: How will i handle creating vs editing of filter rules?
        // if (m_editing == false && ImGui::Button("Create"))
        // {
        //     auto [filterIndex, _] = m_filterRuleState.getIndices();
        //     // NOTE: Could allow proper FilterRule<T> copying if this draw function was made into a template.
        //     addColumnFilterRule.invoke(m_editorColumn, m_editorFilterGroupIndex, filterIndex, m_filterRuleState.getFilterRule());
        //     ImGui::CloseCurrentPopup();
        // }

		ImGui::EndPopup();
	}
}

void FilterEditorSubGui::open_edit(size_t column, size_t filterGroupIndex, const ImRect& avoidRect)
{
    if (m_scheduleCore->existsColumnAtIndex(column) == false) { return; }
    if (m_scheduleCore->getColumn(column)->hasFilterGroupAt(filterGroupIndex) == false) { return; }

	m_editorColumn = column;
    m_editorFilterGroupIndex = filterGroupIndex;
    m_editing = true;
	m_avoidRect = avoidRect;

    DateContainer currentDate = DateContainer::getCurrentSystemDate();
    m_viewedYear = currentDate.getTime().tm_year;
    m_viewedMonth = currentDate.getTime().tm_mon;

    m_filterGroupState.setup(m_scheduleCore->getColumn(column)->type, m_scheduleCore->getColumn(column)->getFilterGroupsConst().at(filterGroupIndex));

	ImGui::OpenPopup("FilterGroup Editor");
}

void FilterEditorSubGui::open_create(size_t column, const ImRect& avoidRect)
{ 
    if (m_scheduleCore->existsColumnAtIndex(column) == false) { return; }

	m_editorColumn = column;
    m_editing = false;
	m_avoidRect = avoidRect;

    DateContainer currentDate = DateContainer::getCurrentSystemDate();
    m_viewedYear = currentDate.getTime().tm_year;
    m_viewedMonth = currentDate.getTime().tm_mon;

    SCHEDULE_TYPE columnType = m_scheduleCore->getColumn(column)->type;

    m_filterGroupState.setup(columnType, FilterGroup());

	ImGui::OpenPopup("FilterGroup Editor");
}

void FilterEditorSubGui::close()
{
    if (ImGui::IsPopupOpen("FilterGroup Editor"))
    {
        ImGui::CloseCurrentPopup();
    }
}

size_t FilterEditorSubGui::getColumn() const
{
	return m_editorColumn;
}