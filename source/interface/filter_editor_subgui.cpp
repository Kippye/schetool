#include <format>
#include "filter_editor_subgui.h"
#include "gui_templates.h"
#include "schedule_constants.h"

using filter_consts::TypeComparisonInfo;


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


void FilterGroupEditorState::setup(SCHEDULE_TYPE type, size_t columnIndex, size_t filterGroupIndex, FilterGroup filterGroup)
{
    m_type = type;
    m_columnIndex = columnIndex;
    m_filterGroupIndex = filterGroupIndex;
    m_filterGroup = filterGroup;
    m_isValid = true;
}

SCHEDULE_TYPE FilterGroupEditorState::getType() const
{
    return m_type;
}

size_t FilterGroupEditorState::getColumnIndex() const
{
    return m_columnIndex;
}

size_t FilterGroupEditorState::getFilterGroupIndex() const
{
    return m_filterGroupIndex;   
}

FilterGroup& FilterGroupEditorState::getFilterGroup()
{
    return m_filterGroup;
}

bool FilterGroupEditorState::getIsValid() const
{
    return m_isValid;
}


FilterRuleEditorSubGui::FilterRuleEditorSubGui(const char* ID, FilterGroupEditorState& filterGroupState) : Gui(ID), m_filterGroupState(filterGroupState)
{

}

void FilterRuleEditorSubGui::draw(Window& window, Input& input)
{
    if (ImGui::BeginPopupEx(ImGui::GetID("FilterRule Editor"), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
	{
        if (m_filterRuleState.getIsValid() == false)
        {
            printf("FilterRuleEditorSubGui::draw(): FilterRuleEditorState is not valid.\n");
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
            if (ImGui::BeginCombo("##filterRuleEditorComparison", filter_consts::comparisonStrings.at(currentComparison)))
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

        gui_templates::TextWithBackground("%s", m_columnName.c_str());

        ImGui::SameLine();

        tm formatTime;
        auto [filterIndex, filterRuleIndex] = m_filterRuleState.getIndices();
        Filter& filter = m_filterGroupState.getFilterGroup().getFilter(filterIndex);

		switch(m_filterRuleState.getType())
		{
            case(SCH_BOOL):
            {
                displayComparisonCombo(SCH_BOOL);

                ImGui::SameLine();

                bool newValue = m_filterRuleState.getFilterRule().getPassValue<bool>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<bool>(); 

                if (ImGui::Checkbox("##filterRuleEditor", &newValue))
                {
                    m_filterRuleState.getFilterRule().setPassValue(newValue);
                    if (m_editing == true)
                    {
                        filter.replaceRule(filterRuleIndex, m_filterRuleState.getFilterRule().getAsType<bool>());
                        editColumnFilterRule.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), filterIndex, filterRuleIndex, FilterRuleContainer().fill(prevFilter), m_filterRuleState.getFilterRule());
                    }
                }
                break;
            }
            case(SCH_NUMBER):
            {
                displayComparisonCombo(SCH_NUMBER);

                ImGui::SameLine();

                int newValue = m_filterRuleState.getFilterRule().getPassValue<int>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<int>(); 

                if (ImGui::InputInt("##filterRuleEditor", &newValue, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    m_filterRuleState.getFilterRule().setPassValue(newValue);
                    if (m_editing == true)
                    {
                        filter.replaceRule(filterRuleIndex, m_filterRuleState.getFilterRule().getAsType<int>());
                        editColumnFilterRule.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), filterIndex, filterRuleIndex, FilterRuleContainer().fill(prevFilter), m_filterRuleState.getFilterRule());
                    }
                }
                break;
            }
            case(SCH_DECIMAL):
            {
                displayComparisonCombo(SCH_DECIMAL);

                ImGui::SameLine();

                double newValue = m_filterRuleState.getFilterRule().getPassValue<double>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<double>(); 
                
                if (ImGui::InputDouble("##filterRuleEditor", &newValue, 0.0, 0.0, "%.15g", ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    m_filterRuleState.getFilterRule().setPassValue(newValue);
                    if (m_editing == true)
                    {
                        filter.replaceRule(filterRuleIndex, m_filterRuleState.getFilterRule().getAsType<double>());
                        editColumnFilterRule.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), filterIndex, filterRuleIndex, FilterRuleContainer().fill(prevFilter), m_filterRuleState.getFilterRule());
                    }
                }
                break;
            }
            case(SCH_TEXT):
            {
                displayComparisonCombo(SCH_TEXT); 

                std::string value = m_filterRuleState.getFilterRule().getPassValue<std::string>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<std::string>(); 
                value.reserve(schedule_consts::ELEMENT_TEXT_MAX_LENGTH);
                char* buf = value.data();
                //ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
                if (ImGui::InputTextMultiline("##filterRuleEditor", buf, value.capacity(), ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
                {
                    m_filterRuleState.getFilterRule().setPassValue(std::string(buf));
                    if (m_editing == true)
                    {
                        filter.replaceRule(filterRuleIndex, m_filterRuleState.getFilterRule().getAsType<std::string>());
                        editColumnFilterRule.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), filterIndex, filterRuleIndex, FilterRuleContainer().fill(prevFilter), m_filterRuleState.getFilterRule());
                    }
                }

                break;
            }
            case(SCH_SELECT):
            {
				SelectContainer value = m_filterRuleState.getFilterRule().getPassValue<SelectContainer>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<SelectContainer>(); 

                displayComparisonCombo(SCH_SELECT);

                auto selection = value.getSelection();
                // TODO: Get select options somehow
				// const std::vector<std::string>& optionNames = m_scheduleCore->getColumnSelectOptions(m_editorColumn).getOptions();

                // Options
				// for (size_t i = 0; i < optionNames.size(); i++)
				// {
				// 	bool selected = selection.find(i) != selection.end();

                //     if (ImGui::Checkbox(std::string("##SelectFilterEditorCheck").append(std::to_string(i)).c_str(), &selected))
                //     {
                //         value.setSelected(i, selected);
                //         m_filterRuleState.getFilterRule().getAsType<SelectContainer>()->setPassValue(value);
                //         invokeFilterEditEvent<SelectContainer>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<SelectContainer>());
                //     }

                //     ImGui::SameLine();

				// 	std::string optionName = std::string(optionNames[i]);

				// 	if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
				// 	{
				// 		value.setSelected(i, selected);
                //         m_filterRuleState.getFilterRule().getAsType<SelectContainer>()->setPassValue(value);
                //         invokeFilterEditEvent<SelectContainer>(prevFilter, *m_filterRuleState.getFilterRule().getAsType<SelectContainer>());
				// 	}
				// }
                break;
            }
            case(SCH_WEEKDAY):
            {
				WeekdayContainer value = m_filterRuleState.getFilterRule().getPassValue<WeekdayContainer>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<WeekdayContainer>(); 

                displayComparisonCombo(SCH_WEEKDAY);

                if (m_filterRuleState.getFilterRule().getComparison() != Comparison::ContainsToday)
                {
                    auto selection = value.getSelection();
                    const std::vector<std::string>& optionNames = schedule_consts::weekdayNames;
                    // Options
                    for (size_t i = 0; i < optionNames.size(); i++)
                    {
                        bool selected = selection.find(i) != selection.end();

                        auto setCurrentOptionSelected = [&](bool newSelected)
                        {
                            value.setSelected(i, newSelected);
                            m_filterRuleState.getFilterRule().setPassValue(value);
                            if (m_editing == true)
                            {
                                filter.replaceRule(filterRuleIndex, m_filterRuleState.getFilterRule().getAsType<WeekdayContainer>());
                                editColumnFilterRule.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), filterIndex, filterRuleIndex, FilterRuleContainer().fill(prevFilter), m_filterRuleState.getFilterRule());
                            }
                        };

                        if (ImGui::Checkbox(std::string("##WeekdayFilterEditorCheck").append(std::to_string(i)).c_str(), &selected))
                        {
                            setCurrentOptionSelected(selected);
                        }

                        ImGui::SameLine();

                        std::string optionName = std::string(optionNames[i]);

                        ImGui::PushStyleColor(ImGuiCol_Header, gui_colors::dayColors[i]);
                        if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
                        {
                            setCurrentOptionSelected(selected);
                        }
                        ImGui::PopStyleColor(1);
                    }
                }
                break;
            }
            case(SCH_TIME):
            {
                TimeContainer value = m_filterRuleState.getFilterRule().getPassValue<TimeContainer>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<TimeContainer>(); 
                
                displayComparisonCombo(SCH_TIME);

                ImGui::SameLine();

                if (gui_templates::TimeEditor(value))
                {
                    m_filterRuleState.getFilterRule().setPassValue(value);
                    if (m_editing == true)
                    {
                        filter.replaceRule(filterRuleIndex, m_filterRuleState.getFilterRule().getAsType<TimeContainer>());
                        editColumnFilterRule.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), filterIndex, filterRuleIndex, FilterRuleContainer().fill(prevFilter), m_filterRuleState.getFilterRule());
                    }
                }
                break;
            }
            case(SCH_DATE):
            {
                DateContainer value = m_filterRuleState.getFilterRule().getPassValue<DateContainer>();
                auto prevFilter = m_filterRuleState.getFilterRule().getAsType<DateContainer>(); 

                auto [_comparisonChanged, newComparison] = displayComparisonCombo(SCH_DATE);

                // Display the date of the current Date element, UNLESS the comparison is IsEmpty
                if (newComparison != Comparison::IsEmpty)
                {
                    ImGui::SameLine();
                    gui_templates::TextWithBackground("%s##filterRuleEditor", newComparison == Comparison::IsRelativeToToday ? "Today" : value.getString().c_str());
                }

                if (gui_templates::DateEditor(value, m_viewedYear, m_viewedMonth, false, false))
                {
                    // Choosing a specific date makes the filter no longer relative
                    m_filterRuleState.getFilterRule().setComparison(Comparison::Is);

                    m_filterRuleState.getFilterRule().setPassValue(value);
                    if (m_editing == true)
                    {
                        filter.replaceRule(filterRuleIndex, m_filterRuleState.getFilterRule().getAsType<DateContainer>());
                        editColumnFilterRule.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), filterIndex, filterRuleIndex, FilterRuleContainer().fill(prevFilter), m_filterRuleState.getFilterRule());
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

        if (m_editing == false && ImGui::Button("Create"))
        {
            switch(m_filterRuleState.getType())
            {
                case(SCH_BOOL):
                    filter.addRule(m_filterRuleState.getFilterRule().getAsType<bool>());
                    break;
                case(SCH_NUMBER):
                    filter.addRule(m_filterRuleState.getFilterRule().getAsType<int>());
                    break;
                case(SCH_DECIMAL):
                    filter.addRule(m_filterRuleState.getFilterRule().getAsType<double>());
                    break;
                case(SCH_TEXT):
                    filter.addRule(m_filterRuleState.getFilterRule().getAsType<std::string>());
                    break;
                case(SCH_SELECT):
                    filter.addRule(m_filterRuleState.getFilterRule().getAsType<SelectContainer>());
                    break;
                case(SCH_WEEKDAY):
                    filter.addRule(m_filterRuleState.getFilterRule().getAsType<WeekdayContainer>());
                    break;
                case(SCH_TIME):
                    filter.addRule(m_filterRuleState.getFilterRule().getAsType<TimeContainer>());
                    break;
                case(SCH_DATE):
                    filter.addRule(m_filterRuleState.getFilterRule().getAsType<DateContainer>());
                    break;                
                default:
                {
                    printf("FilterRuleEditorSubGui::draw(): Creating filters of type %d has not been implemented\n", m_filterRuleState.getType());
                    return;
                }
            }
            addColumnFilterRule.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), filterIndex, m_filterRuleState.getFilterRule());
            ImGui::CloseCurrentPopup();
        }

		ImGui::EndPopup();
	}

    if (m_openNextFrame)
    {
        ImGui::OpenPopup("FilterRule Editor");
        m_openNextFrame = false;
    }
}

void FilterRuleEditorSubGui::openEdit(SCHEDULE_TYPE type, const std::string& columnName, size_t filterIndex, size_t ruleIndex, const ImRect& avoidRect)
{
    if (m_filterGroupState.getFilterGroup().hasFilterAt(filterIndex) == false) { return; }
    if (ruleIndex < m_filterGroupState.getFilterGroup().getFilter(filterIndex).getRuleCount() == false) { return; }
 
    m_editing = true;
    m_columnName = columnName;
    m_avoidRect = avoidRect;

    DateContainer currentDate = DateContainer::getCurrentSystemDate();
    m_viewedYear = currentDate.getTime().tm_year;
    m_viewedMonth = currentDate.getTime().tm_mon;

    // Set up m_filterRuleState and open the popup
    switch(type)
    {
        case(SCH_BOOL):
            m_filterRuleState.setup(type, filterIndex, ruleIndex, m_filterGroupState.getFilterGroup().getFilter(filterIndex).getRule(ruleIndex).getAsType<bool>());
            break;
        case(SCH_NUMBER):
            m_filterRuleState.setup(type, filterIndex, ruleIndex, m_filterGroupState.getFilterGroup().getFilter(filterIndex).getRule(ruleIndex).getAsType<int>());
            break;
        case(SCH_DECIMAL):
             m_filterRuleState.setup(type, filterIndex, ruleIndex, m_filterGroupState.getFilterGroup().getFilter(filterIndex).getRule(ruleIndex).getAsType<double>());
            break;
        case(SCH_TEXT):
            m_filterRuleState.setup(type, filterIndex, ruleIndex, m_filterGroupState.getFilterGroup().getFilter(filterIndex).getRule(ruleIndex).getAsType<std::string>());
            break;
        case(SCH_SELECT):
            m_filterRuleState.setup(type, filterIndex, ruleIndex, m_filterGroupState.getFilterGroup().getFilter(filterIndex).getRule(ruleIndex).getAsType<SelectContainer>());
            break;
        case(SCH_WEEKDAY):
            m_filterRuleState.setup(type, filterIndex, ruleIndex, m_filterGroupState.getFilterGroup().getFilter(filterIndex).getRule(ruleIndex).getAsType<WeekdayContainer>());
            break;
        case(SCH_TIME):
            m_filterRuleState.setup(type, filterIndex, ruleIndex, m_filterGroupState.getFilterGroup().getFilter(filterIndex).getRule(ruleIndex).getAsType<TimeContainer>());
            break;
        case(SCH_DATE):
            m_filterRuleState.setup(type, filterIndex, ruleIndex, m_filterGroupState.getFilterGroup().getFilter(filterIndex).getRule(ruleIndex).getAsType<DateContainer>());
            break;
        default:
        {
            printf("FilterRuleEditorSubGui::openEdit(): Editing a FilterRule of type %d has not been implemented\n", type);
            return;
        }
    }

    m_openNextFrame = true;
}

void FilterRuleEditorSubGui::openCreate(SCHEDULE_TYPE type, const std::string& columnName, size_t filterIndex, const ImRect& avoidRect)
{
    if (m_filterGroupState.getFilterGroup().hasFilterAt(filterIndex) == false) { return; }

    m_editing = false;
    m_columnName = columnName;
	m_avoidRect = avoidRect;

    DateContainer currentDate = DateContainer::getCurrentSystemDate();
    m_viewedYear = currentDate.getTime().tm_year;
    m_viewedMonth = currentDate.getTime().tm_mon;

    Filter& filter = m_filterGroupState.getFilterGroup().getFilter(filterIndex);

    // Set up m_filterRuleState and open the popup
    switch(type)
    {
        case(SCH_BOOL):
        {
            m_filterRuleState.setup(type, filterIndex, filter.getRuleCount(), FilterRule<bool>(false));
            break;
        }
        case(SCH_NUMBER):
        {
            m_filterRuleState.setup(type, filterIndex, filter.getRuleCount(), FilterRule<int>(0));
            break;
        }
        case(SCH_DECIMAL):
        {
            m_filterRuleState.setup(type, filterIndex, filter.getRuleCount(), FilterRule<double>(0.0));
            break;
        }
        case(SCH_TEXT):
        {
            m_filterRuleState.setup(type, filterIndex, filter.getRuleCount(), FilterRule<std::string>(""));
            break;
        }
        case(SCH_SELECT):
        {
            m_filterRuleState.setup(type, filterIndex, filter.getRuleCount(), FilterRule<SelectContainer>(SelectContainer()));
            break;
        }
        case(SCH_WEEKDAY):
        {
            m_filterRuleState.setup(type, filterIndex, filter.getRuleCount(), FilterRule<WeekdayContainer>(WeekdayContainer()));
            break;
        }
        case(SCH_TIME):
        {
            m_filterRuleState.setup(type, filterIndex, filter.getRuleCount(), FilterRule<TimeContainer>(TimeContainer(0, 0)));
            break;
        }
        case(SCH_DATE):
        {
            // default date comparison to IsRelativeToToday
            FilterRule<DateContainer> dateRule = FilterRule<DateContainer>(DateContainer::getCurrentSystemDate());
            dateRule.setComparison(Comparison::IsRelativeToToday);
            m_filterRuleState.setup(type, filterIndex, filter.getRuleCount(), dateRule);
            break;
        }
        default:
        {
            printf("FilterRuleEditorSubGui::openCreate(): Creating filters for type %d has not been implemented\n", type);
            return;
        }
    }

    m_openNextFrame = true;
}

void FilterRuleEditorSubGui::close()
{
    if (ImGui::IsPopupOpen("FilterRule Editor"))
    {
        ImGui::CloseCurrentPopup();
    }
}


FilterEditorSubGui::FilterEditorSubGui(const char* ID, const ScheduleCore* scheduleCore, ScheduleEvents& scheduleEvents) : Gui(ID) 
{
	m_scheduleCore = scheduleCore;
    scheduleEvents.columnAdded.addListener(columnAddedListener);
    scheduleEvents.columnRemoved.addListener(columnRemovedListener);
	addSubGui("FilterRuleEditorSubGui", new FilterRuleEditorSubGui("FilterRuleEditorSubGui", m_filterGroupState));
}

void FilterEditorSubGui::draw(Window& window, Input& input)
{
	if (ImGui::BeginPopupEx(ImGui::GetID("FilterGroup Editor"), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize))
	{
        if (m_scheduleCore->existsColumnAtIndex(m_filterGroupState.getColumnIndex()) == false)
        {
            printf("FilterEditorSubGui::draw(): There is no Column at the column index %zu\n", m_filterGroupState.getColumnIndex());
            ImGui::EndPopup();
            close();
            return;
        }
        if (m_scheduleCore->getColumn(m_filterGroupState.getColumnIndex())->type != m_filterGroupState.getType())
        {
            printf("FilterEditorSubGui::draw(): The types of the Column (%d) and the editor's filter state (%d) do not match!\n", m_scheduleCore->getColumn(m_filterGroupState.getColumnIndex())->type, m_filterGroupState.getType());
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

		bool isPermanentColumn = m_scheduleCore->getColumn(m_filterGroupState.getColumnIndex())->permanent;
		tm formatTime;

        std::string groupName = m_filterGroupState.getFilterGroup().getName();
        groupName.reserve(schedule_consts::FILTER_GROUP_NAME_MAX_LENGTH);
        char* buf = groupName.data();
        if (ImGui::InputText("##FilterGroupNameInput", buf, schedule_consts::FILTER_GROUP_NAME_MAX_LENGTH))
        {
            m_filterGroupState.getFilterGroup().setName(buf);
            setColumnFilterGroupName.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), std::string(buf)); 
        }

        ImGui::SameLine();

        if (ImGui::SmallButton("X##RemoveFilterGroup"))
        {
            removeColumnFilterGroup.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex());
            ImGui::CloseCurrentPopup();
        }

        auto drawFilterRule = [&](size_t filterIndex, size_t ruleIndex)
        {
            Filter& filter = m_filterGroupState.getFilterGroup().getFilter(filterIndex);
            if (ImGui::Button(std::format("{}##{};{}", filter.getRule(ruleIndex).getString(), filterIndex, ruleIndex).c_str()))
            {
                // Open the filter rule editor to edit this rule
                if (auto filterRuleEditor = getSubGui<FilterRuleEditorSubGui>("FilterRuleEditorSubGui"))
                {
                    // TODO: Pass correct avoid rect
                    filterRuleEditor->openEdit(m_filterGroupState.getType(), m_scheduleCore->getColumn(m_filterGroupState.getColumnIndex())->name, filterIndex, ruleIndex, m_avoidRect);
                }
            }
            ImGui::SameLine();
            // Remove FilterRule button
            if (ImGui::Button(std::format("X##RemoveFilterRule{};{}", filterIndex, ruleIndex).c_str()))
            {
                m_filterGroupState.getFilterGroup().getFilter(filterIndex).removeRule(ruleIndex);
                removeColumnFilterRule.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), filterIndex, ruleIndex);
            }
        };

        // Display all Filters
        for (size_t f = 0; f < m_filterGroupState.getFilterGroup().getFilters().size(); f++)
        {
            Filter& filter = m_filterGroupState.getFilterGroup().getFilter(f);

            ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(FLT_MAX, /* Calculate size using number of FilterRules */ FLT_MAX));
            if (ImGui::BeginChild(std::format("###Filter{}", f).c_str(), ImVec2(-FLT_MIN, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY))
            {
                // Display all contained FilterRules
                for (size_t r = 0; r < filter.getRules().size(); r++)
                {
                    drawFilterRule(f, r);
                    // display operator between each rule except the last
                    if (r < filter.getRules().size() - 1)
                    {
                        if (ImGui::BeginCombo(std::format("##FilterOperator{};{}", f, r).c_str(), schedule_consts::logicalOperatorStrings.at(filter.getOperatorType())))
                        {
                            for (const auto& [logicalOperator, operatorString]: schedule_consts::logicalOperatorStrings)
                            {
                                bool isSelected = filter.getOperatorType() == logicalOperator;
                                if (ImGui::Selectable(operatorString, isSelected))
                                {
                                    filter.setOperator(logicalOperator);
                                }
                            }

                            ImGui::EndCombo();
                        }
                    }
                }
                // Add new rule button
                if (ImGui::Button(std::format("+ Add rule##{}", f).c_str()))
                {
                    // DONT add a rule. Open the FilterRule editor with create. If the user creates it, THEN it will be added!
                    if (auto filterRuleEditor = getSubGui<FilterRuleEditorSubGui>("FilterRuleEditorSubGui"))
                    {
                        // TODO: Pass correct avoid rect
                        filterRuleEditor->openCreate(m_filterGroupState.getType(), m_scheduleCore->getColumn(m_filterGroupState.getColumnIndex())->name, f, m_avoidRect);
                    }
                }

                ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                // Remove filter button
                if (ImGui::Button(std::format("X##RemoveFilter{}", f).c_str()))
                {
                    m_filterGroupState.getFilterGroup().removeFilter(f);
                    removeColumnFilter.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), f);
                }
            }
            ImGui::EndChild();
            // display operator between each Filter except the last
            if (f < m_filterGroupState.getFilterGroup().getFilters().size() - 1)
            {
                if (ImGui::BeginCombo(std::format("##FilterGroupOperator{}", f).c_str(), schedule_consts::logicalOperatorStrings.at(m_filterGroupState.getFilterGroup().getOperatorType())))
                {
                    for (const auto& [logicalOperator, operatorString]: schedule_consts::logicalOperatorStrings)
                    {
                        bool isSelected = m_filterGroupState.getFilterGroup().getOperatorType() == logicalOperator;
                        if (ImGui::Selectable(operatorString, isSelected))
                        {
                            m_filterGroupState.getFilterGroup().setOperator(logicalOperator);
                        }
                    }

                    ImGui::EndCombo();
                }
            }
        }
        // Add new filter button
        if (ImGui::Button("+ Add filter"))
        {
            m_filterGroupState.getFilterGroup().addFilter(Filter());
            addColumnFilter.invoke(m_filterGroupState.getColumnIndex(), m_filterGroupState.getFilterGroupIndex(), Filter());
        }

        if (auto filterRuleEditor = getSubGui<FilterRuleEditorSubGui>("FilterRuleEditorSubGui"))
        {
            filterRuleEditor->draw(window, input);
        }
		ImGui::EndPopup();
	}
}

void FilterEditorSubGui::openGroupEdit(size_t column, size_t filterGroupIndex, const ImRect& avoidRect)
{
    if (m_scheduleCore->existsColumnAtIndex(column) == false) { return; }
    if (m_scheduleCore->getColumn(column)->hasFilterGroupAt(filterGroupIndex) == false) { return; }

	m_avoidRect = avoidRect;

    m_filterGroupState.setup(m_scheduleCore->getColumn(column)->type, column, filterGroupIndex, m_scheduleCore->getColumn(column)->getFilterGroupsConst().at(filterGroupIndex));

	ImGui::OpenPopup("FilterGroup Editor");
}

void FilterEditorSubGui::createGroupAndEdit(size_t column, const ImRect& avoidRect)
{ 
    if (m_scheduleCore->existsColumnAtIndex(column) == false) { return; }

    addColumnFilterGroup.invoke(column, FilterGroup());

    // Event has no listeners or they failed somewhere, can't edit a non-existant FilterGroup so quit.
    if (m_scheduleCore->getColumn(column)->getFilterGroupsConst().size() == 0) { return; }

    // NOTE: Just assuming that the FilterGroup was actually added..
    // Not the best idea, but eh.
    openGroupEdit(column, m_scheduleCore->getColumn(column)->getFilterGroupCount() - 1, avoidRect);
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
	return m_filterGroupState.getColumnIndex();
}