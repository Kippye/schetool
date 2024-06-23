#include "filter_editor_subgui.h"
#include "gui_templates.h"
#include "schedule_constants.h"

using filter_consts::TypeComparisonInfo;

void EditorFilterState::setFilter(const std::shared_ptr<FilterBase>& filter)
{
    m_filter = filter;
}

std::shared_ptr<FilterBase> EditorFilterState::getFilterBase()
{
    return m_filter;
}

void EditorFilterState::setType(SCHEDULE_TYPE type)
{
    if (type == SCH_LAST) { return; }
    m_type = type;
}

SCHEDULE_TYPE EditorFilterState::getType() const
{
    return m_type;
}

bool EditorFilterState::hasValidFilter() const
{
    if (m_filter)
    {
        return true;
    }
    else
    {
        return false;
    }
}


FilterEditorSubGui::FilterEditorSubGui(const char* ID, const ScheduleCore* scheduleCore) : Gui(ID) 
{
	m_scheduleCore = scheduleCore;
}

void FilterEditorSubGui::draw(Window& window, Input& input)
{
	m_openLastFrame = m_openThisFrame;

	if (ImGui::BeginPopupEx(ImGui::GetID("Filter Editor"), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
	{
        if (m_scheduleCore->existsColumnAtIndex(m_editorColumn) == false)
        {
            printf("FilterEditorSubGui::draw(): There is no Column at the editor column index %zu\n", m_editorColumn);
            ImGui::EndPopup();
            return;
        }
        if (m_scheduleCore->getColumn(m_editorColumn)->type != m_filterState.getType())
        {
            printf("FilterEditorSubGui::draw(): The types of the Column (%d) and the editor's filter state (%d) do not match!\n", m_scheduleCore->getColumn(m_editorColumn)->type, m_filterState.getType());
            ImGui::EndPopup();
            return;
        }

		// TODO clean up / make the positioning more precise!
		ImGuiDir dir = ImGuiDir_Down;
		ImGuiWindow* popup = ImGui::GetCurrentWindow();
		ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
		ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
		//ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(popup->Pos, autoFitSize, &popup->AutoPosLastDirection, r_outer, m_avoidRect, ImGuiPopupPositionPolicy_Default));
		//return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, ImRect(window->Pos, window->Pos), ImGuiPopupPositionPolicy_Default); // Ideally we'd disable r_avoid here

		bool isPermanentColumn = m_scheduleCore->getColumn(m_editorColumn)->permanent;
		tm formatTime;

        // LAMBDA.
        // Display a Selectable combo for choosing the comparison with options for the provided type.
        // Returns a pair containing a bool (whether the comparison was changed) and a Comparison (the selected Comparison)
        auto displayComparisonCombo = [&](SCHEDULE_TYPE type) -> std::pair<bool, Comparison>
        {
            Comparison currentComparison = m_filterState.getFilterBase()->getComparison();
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
                        m_filterState.getFilterBase()->setComparison(comparisonInfo.comparisons.at(i));
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
            return { selectionChanged, m_filterState.getFilterBase()->getComparison() };
        };
    
        // LAMBDA
        auto displayRemoveFilterButton = [&]() 
        {
            if (ImGui::SmallButton("X##removeFilter"))
            {
                removeColumnFilter.invoke(m_editorColumn, m_editorFilterIndex);
                ImGui::CloseCurrentPopup();
            }
        };

        gui_templates::TextWithBackground("%s", m_scheduleCore->getColumn(m_editorColumn)->name.c_str());

        ImGui::SameLine();

		switch(m_filterState.getType())
		{
            case(SCH_BOOL):
            {
                displayComparisonCombo(SCH_BOOL);

                ImGui::SameLine();

                bool newValue = m_filterState.getFilter<bool>()->getPassValue();
                auto prevFilter = *m_filterState.getFilter<bool>(); 

                if (ImGui::Checkbox(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterState.getFilter<bool>()->setPassValue(newValue);
                    invokeFilterEditEvent<bool>(prevFilter, *m_filterState.getFilter<bool>());
                }
                break;
            }
            case(SCH_NUMBER):
            {
                displayComparisonCombo(SCH_NUMBER);

                ImGui::SameLine();

                int newValue = m_filterState.getFilter<int>()->getPassValue();
                auto prevFilter = *m_filterState.getFilter<int>(); 

                if (ImGui::InputInt(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterState.getFilter<int>()->setPassValue(newValue);
                    invokeFilterEditEvent<int>(prevFilter, *m_filterState.getFilter<int>());
                }
                break;
            }
            case(SCH_DECIMAL):
            {
                displayComparisonCombo(SCH_DECIMAL);

                ImGui::SameLine();

                double newValue = m_filterState.getFilter<double>()->getPassValue();
                auto prevFilter = *m_filterState.getFilter<double>(); 
                
                if (ImGui::InputDouble(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterState.getFilter<double>()->setPassValue(newValue);
                    invokeFilterEditEvent<double>(prevFilter, *m_filterState.getFilter<double>());
                }
                break;
            }
            case(SCH_TEXT):
            {
                displayComparisonCombo(SCH_TEXT);

                std::string value = m_filterState.getFilter<std::string>()->getPassValue();
                auto prevFilter = *m_filterState.getFilter<std::string>(); 
                value.reserve(ELEMENT_TEXT_MAX_LENGTH);
                char* buf = value.data();
                //ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
                if (ImGui::InputTextMultiline(std::string("##filterEditor").append(std::to_string(m_editorColumn)).c_str(), buf, value.capacity(), ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
                {
                    m_filterState.getFilter<std::string>()->setPassValue(std::string(buf));
                    invokeFilterEditEvent<std::string>(prevFilter, *m_filterState.getFilter<std::string>());
                }

                break;
            }
            case(SCH_SELECT):
            {
				SelectContainer value = m_filterState.getFilter<SelectContainer>()->getPassValue();
                auto prevFilter = *m_filterState.getFilter<SelectContainer>(); 

                displayComparisonCombo(SCH_SELECT);

                // Remove button
                if (m_editing == true)
                {
                    ImGui::SameLine();
                    displayRemoveFilterButton();
                }

                auto selection = value.getSelection();
				const std::vector<std::string>& optionNames = m_scheduleCore->getColumnSelectOptions(m_editorColumn).getOptions();

                // Options
				for (size_t i = 0; i < optionNames.size(); i++)
				{
					bool selected = selection.find(i) != selection.end();

                    if (ImGui::Checkbox(std::string("##SelectFilterEditorCheck").append(std::to_string(i)).c_str(), &selected))
                    {
                        value.setSelected(i, selected);
                        m_filterState.getFilter<SelectContainer>()->setPassValue(value);
                        invokeFilterEditEvent<SelectContainer>(prevFilter, *m_filterState.getFilter<SelectContainer>());
                    }

                    ImGui::SameLine();

					std::string optionName = std::string(optionNames[i]);

					if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
					{
						value.setSelected(i, selected);
                        m_filterState.getFilter<SelectContainer>()->setPassValue(value);
                        invokeFilterEditEvent<SelectContainer>(prevFilter, *m_filterState.getFilter<SelectContainer>());
					}
				}
                break;
            }
            case(SCH_WEEKDAY):
            {
				WeekdayContainer value = m_filterState.getFilter<WeekdayContainer>()->getPassValue();
                auto prevFilter = *m_filterState.getFilter<WeekdayContainer>(); 

                displayComparisonCombo(SCH_WEEKDAY);

                // Remove button
                if (m_editing == true)
                {
                    ImGui::SameLine();
                    displayRemoveFilterButton();
                }

                if (m_filterState.getFilterBase()->getComparison() != Comparison::ContainsToday)
                {
                    auto selection = value.getSelection();
                    const std::vector<std::string>& optionNames = schedule_consts::weekdayNames;
                    // Options
                    for (size_t i = 0; i < optionNames.size(); i++)
                    {
                        bool selected = selection.find(i) != selection.end();

                        if (ImGui::Checkbox(std::string("##WeekdayFilterEditorCheck").append(std::to_string(i)).c_str(), &selected))
                        {
                            value.setSelected(i, selected);
                            m_filterState.getFilter<WeekdayContainer>()->setPassValue(value);
                            invokeFilterEditEvent<WeekdayContainer>(prevFilter, *m_filterState.getFilter<WeekdayContainer>());
                        }

                        ImGui::SameLine();

                        std::string optionName = std::string(optionNames[i]);

                        ImGui::PushStyleColor(ImGuiCol_Header, gui_colors::dayColors[i]);
                        if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
                        {
                            value.setSelected(i, selected);
                            m_filterState.getFilter<WeekdayContainer>()->setPassValue(value);
                            invokeFilterEditEvent<WeekdayContainer>(prevFilter, *m_filterState.getFilter<WeekdayContainer>());
                        }
                        ImGui::PopStyleColor(1);
                    }
                }
                break;
            }
            case(SCH_TIME):
            {
                TimeContainer value = m_filterState.getFilter<TimeContainer>()->getPassValue();
                auto prevFilter = *m_filterState.getFilter<TimeContainer>(); 
                
                displayComparisonCombo(SCH_TIME);

                ImGui::SameLine();

                if (gui_templates::TimeEditor(value))
                {
                    m_filterState.getFilter<TimeContainer>()->setPassValue(value);
                    invokeFilterEditEvent<TimeContainer>(prevFilter, *m_filterState.getFilter<TimeContainer>());
                }
 
                break;
            }
            case(SCH_DATE):
            {
                DateContainer value = m_filterState.getFilter<DateContainer>()->getPassValue();
                auto prevFilter = *m_filterState.getFilter<DateContainer>(); 

                auto [_comparisonChanged, newComparison] = displayComparisonCombo(SCH_DATE);

                // Switch between a relative and absolute filter
                // if (ImGui::Combo("##filterEditorDateMode", &comparisonOptionTemp, m_typeComparisonOptions.getOptions(SCH_DATE).string))
                // {
                //     if ((DateMode)comparisonOptionTemp == DateMode::Absolute)
                //     {
                //         m_filterState.getFilter<DateContainer>()->setPassValue(DateContainer(value.getTime(), false));
                //     }
                //     else
                //     {
                //         m_filterState.getFilter<DateContainer>()->setPassValue(DateContainer(value.getTime(), true));
                //     }

                //     m_typeComparisonOptions.setOptionSelection(SCH_DATE, comparisonOptionTemp);
                //     invokeFilterEditEvent<DateContainer>(prevFilter, *m_filterState.getFilter<DateContainer>());
                // }

                ImGui::SameLine();

                // !!! update value, might have been modified above
                value = m_filterState.getFilter<DateContainer>()->getPassValue();

                // Display the date of the current Date element
                gui_templates::TextWithBackground("%s##filterEditor%zu", newComparison == Comparison::IsRelativeToToday ? "Today" : value.getString().c_str(), m_editorColumn);

                // If editing, display the remove filter button here, before the Date editor 
                if (m_editing == true)
                {
                    ImGui::SameLine();
                    displayRemoveFilterButton();
                }

                if (gui_templates::DateEditor(value, m_viewedYear, m_viewedMonth, false))
                {
                    m_filterState.getFilter<DateContainer>()->setComparison(Comparison::Is);
                    // weird lil thing: if a Date was selected, then the Date is no longer relative. so we make a copy using its time but with relative = false
                    m_filterState.getFilter<DateContainer>()->setPassValue(DateContainer(value.getTime()));
                    invokeFilterEditEvent<DateContainer>(prevFilter, *m_filterState.getFilter<DateContainer>());
                }
                
                break;
            }
			default:
            {
                ImGui::EndPopup();
                return;
            }
		}

        std::set<SCHEDULE_TYPE> typesWithCustomRemovePosition = { SCH_SELECT, SCH_WEEKDAY, SCH_DATE };

        if (m_editing == true && typesWithCustomRemovePosition.find(m_filterState.getType()) == typesWithCustomRemovePosition.end())
        {
            ImGui::SameLine();
            displayRemoveFilterButton();
        }

        if (m_editing == false && ImGui::Button("Create"))
        {
            addColumnFilter.invoke(m_editorColumn, m_filterState.getFilterBase());
            ImGui::CloseCurrentPopup();
        }

		m_openThisFrame = true;
		ImGui::EndPopup();
	}
	else
	{
		m_openThisFrame = false;
	}
}

void FilterEditorSubGui::open_edit(size_t column, size_t filterIndex, const ImRect& avoidRect)
{
    if (m_scheduleCore->existsColumnAtIndex(column) == false) { return; }

	m_editorColumn = column;
    m_editorFilterIndex = filterIndex;
    m_editing = true;
	m_avoidRect = avoidRect;

    DateContainer currentDate = DateContainer::getCurrentSystemDate();
    m_viewedYear = currentDate.getTime().tm_year;
    m_viewedMonth = currentDate.getTime().tm_mon;

	m_madeEdits = false;

    m_filterState.setType(m_scheduleCore->getColumn(column)->type);
    m_filterState.setFilter(m_scheduleCore->getColumn(column)->getFiltersConst().at(filterIndex));

	ImGui::OpenPopup("Filter Editor");
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

	m_madeEdits = false;

    SCHEDULE_TYPE columnType = m_scheduleCore->getColumn(column)->type;

    m_filterState.setType(columnType);

    switch(columnType)
    {
        case(SCH_BOOL):
        {
            m_filterState.setFilter(std::make_shared<Filter<bool>>(false));
            break;
        }
        case(SCH_NUMBER):
        {
            m_filterState.setFilter(std::make_shared<Filter<int>>(0));
            break;
        }
        case(SCH_DECIMAL):
        {
            m_filterState.setFilter(std::make_shared<Filter<double>>(0.0));
            break;
        }
        case(SCH_TEXT):
        {
            m_filterState.setFilter(std::make_shared<Filter<std::string>>(std::string("")));
            break;
        }
        case(SCH_SELECT):
        {
            m_filterState.setFilter(std::make_shared<Filter<SelectContainer>>(SelectContainer()));
            break;
        }
        case(SCH_WEEKDAY):
        {
            m_filterState.setFilter(std::make_shared<Filter<WeekdayContainer>>(WeekdayContainer()));
            break;
        }
        case(SCH_TIME):
        {
            m_filterState.setFilter(std::make_shared<Filter<TimeContainer>>(TimeContainer(0, 0)));
            break;
        }
        case(SCH_DATE):
        {
            // default date mode to relative
            m_filterState.setFilter(std::make_shared<Filter<DateContainer>>(DateContainer::getCurrentSystemDate()));
            m_filterState.getFilter<DateContainer>()->setComparison(Comparison::IsRelativeToToday);
            break;
        }
        default:
        {
            printf("FilterEditorSubGui::open_create(%zu): Creating filters for type %d has not been implemented\n", column, columnType);
            return;
        }
    }

	ImGui::OpenPopup("Filter Editor");
}

bool FilterEditorSubGui::getOpenThisFrame() const
{
	return  m_openThisFrame;
}

bool FilterEditorSubGui::getOpenLastFrame() const
{
	return  m_openLastFrame;
}

bool FilterEditorSubGui::getMadeEdits() const
{
	return m_madeEdits;
}

size_t FilterEditorSubGui::getFilterColumn() const
{
	return m_editorColumn;
}