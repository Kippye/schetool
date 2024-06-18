#include "filter_editor_subgui.h"
#include "gui_templates.h"

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
    if (type == SCH_LAST || type == SCH_WEEKDAY) { return; }
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


ComparisonOptions TypeComparisonOptions::getOptions(SCHEDULE_TYPE type)
{
    if (m_comparisonOptions.find(type) == m_comparisonOptions.end()) { printf("TypeComparisonOptions::getComparisonOptions(%d): No comparison options for type.\n", type); return ComparisonOptions("" , 0); }

    return m_comparisonOptions.at(type);
}

int TypeComparisonOptions::getOptionSelection(SCHEDULE_TYPE type)
{
    if (m_selectedOptions.find(type) == m_selectedOptions.end()) { printf("TypeComparisonOptions::getOptionSelection(%d): No selection for type.\n", type); return 0; }
    return m_selectedOptions.at(type);
}

void TypeComparisonOptions::setOptionSelection(SCHEDULE_TYPE type, int selection)
{
    if (m_comparisonOptions.find(type) == m_comparisonOptions.end()) { printf("TypeComparisonOptions::setOptionSelection(%d, %d): No comparison options for type.\n", type, selection); return; }
    if (m_selectedOptions.find(type) == m_selectedOptions.end()) { printf("TypeComparisonOptions::setOptionSelection(%d, %d): No selection for type.\n", type, selection); return; }

    size_t optionCount = getOptions(type).count;

    if (selection >= optionCount) 
    { 
        printf("TypeComparisonOptions::setOptionSelection(%d, %d): Selection higher than amount of options (%td). Clamping it to %td.\n", type, selection, optionCount, optionCount - 1);
        selection = optionCount - 1;
    }

    m_selectedOptions.at(type) = selection;
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
            printf("FilterEditorSubGui::draw(): There is no Column at the editor column index %d\n", m_editorColumn);
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

        // LAMBDA
        auto displayRemoveFilterButton = [&]() 
        {
            if (ImGui::SmallButton("X##removeFilter"))
            {
                removeColumnFilter.invoke(m_editorColumn, m_editorFilterIndex);
                ImGui::CloseCurrentPopup();
            }
        };

        // LAMBDA that invokes editColumnFilter if m_editing
        auto invokeEventIfEditing = [&]()
        {
            if (m_editing)
            {
                editColumnFilter.invoke(m_editorColumn, m_editorFilterIndex, std::make_shared<FilterBase>(*m_filterState.getFilterBase()), m_filterState.getFilterBase());
            }
        };

        gui_templates::TextWithBackground("%s", m_scheduleCore->getColumn(m_editorColumn)->name.c_str());

        ImGui::SameLine();

		switch(m_filterState.getType())
		{
            case(SCH_BOOL):
            {
                bool newValue = m_filterState.getFilter<bool>()->getPassValue();
                if (ImGui::Checkbox(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterState.getFilter<bool>()->setPassValue(newValue);
                    invokeEventIfEditing();
                }
                break;
            }
            case(SCH_NUMBER):
            {
                int newValue = m_filterState.getFilter<int>()->getPassValue();
                if (ImGui::InputInt(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterState.getFilter<int>()->setPassValue(newValue);
                    invokeEventIfEditing();
                }
                break;
            }
            case(SCH_DECIMAL):
            {
                double newValue = m_filterState.getFilter<double>()->getPassValue();
                if (ImGui::InputDouble(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterState.getFilter<double>()->setPassValue(newValue);
                    invokeEventIfEditing();
                }
                break;
            }
            case(SCH_TEXT):
            {
                std::string value = m_filterState.getFilter<std::string>()->getPassValue();
                value.reserve(ELEMENT_TEXT_MAX_LENGTH);
                char* buf = value.data();
                //ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
                if (ImGui::InputTextMultiline(std::string("##filterEditor").append(std::to_string(m_editorColumn)).c_str(), buf, value.capacity(), ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
                {
                    m_filterState.getFilter<std::string>()->setPassValue(std::string(buf));
                    invokeEventIfEditing();
                }

                break;
            }
            case(SCH_SELECT):
            {
				SelectContainer value = m_filterState.getFilter<SelectContainer>()->getPassValue();

                int comparisonOptionTemp = m_typeComparisonOptions.getOptionSelection(SCH_SELECT);

                if (ImGui::Combo("##filterEditorSelectComparison", &comparisonOptionTemp, m_typeComparisonOptions.getOptions(SCH_SELECT).string))
                {
                    m_typeComparisonOptions.setOptionSelection(SCH_SELECT, comparisonOptionTemp);
                }

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
                        invokeEventIfEditing();
                    }

                    ImGui::SameLine();

					std::string optionName = std::string(optionNames[i]);

					if (ImGui::Selectable(optionName.append("##EditorOption").append(std::to_string(i)).c_str(), &selected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
					{
						value.setSelected(i, selected);
                        m_filterState.getFilter<SelectContainer>()->setPassValue(value);
                        invokeEventIfEditing();
					}
				}
                break;
            }
            case(SCH_TIME):
            {
                TimeContainer value = m_filterState.getFilter<TimeContainer>()->getPassValue();
                int comparisonOptionTemp = m_typeComparisonOptions.getOptionSelection(SCH_TIME);

                if (ImGui::Combo("##filterEditorTimeComparison", &comparisonOptionTemp, m_typeComparisonOptions.getOptions(SCH_TIME).string))
                {
                    m_typeComparisonOptions.setOptionSelection(SCH_TIME, comparisonOptionTemp);
                }

                ImGui::SameLine();

                if (gui_templates::TimeEditor(value))
                {
                    m_filterState.getFilter<TimeContainer>()->setPassValue(value);
                    invokeEventIfEditing();
                }
 
                break;
            }
            case(SCH_DATE):
            {
                DateContainer value = m_filterState.getFilter<DateContainer>()->getPassValue();
                int comparisonOptionTemp = m_typeComparisonOptions.getOptionSelection(SCH_DATE);
            
                // Switch between a relative and absolute filter
                if (ImGui::Combo("##filterEditorDateMode", &comparisonOptionTemp, m_typeComparisonOptions.getOptions(SCH_DATE).string))
                {
                    if ((DateMode)comparisonOptionTemp == DateMode::Absolute)
                    {
                        m_filterState.getFilter<DateContainer>()->setPassValue(DateContainer(value.getTime(), false));
                    }
                    else
                    {
                        m_filterState.getFilter<DateContainer>()->setPassValue(DateContainer(value.getTime(), true));
                    }

                    m_typeComparisonOptions.setOptionSelection(SCH_DATE, comparisonOptionTemp);
                }

                ImGui::SameLine();

                // !!! update value, might have been modified above
                value = m_filterState.getFilter<DateContainer>()->getPassValue();

                // Display the date of the current Date element
                gui_templates::TextWithBackground("%s##filterEditor%zu", value.getString().c_str(), m_editorColumn);

                // If editing, display the remove filter button here, before the Date editor 
                if (m_editing == true)
                {
                    ImGui::SameLine();
                    displayRemoveFilterButton();
                }

                if (gui_templates::DateEditor(value, m_viewedYear, m_viewedMonth, false))
                {
                    m_typeComparisonOptions.setOptionSelection(SCH_DATE, 1);
                    m_selectedDateMode = DateMode::Absolute;
                    // weird lil thing: if a Date was selected, then the Date is no longer relative. so we make a copy using its time but with relative = false
                    m_filterState.getFilter<DateContainer>()->setPassValue(DateContainer(value.getTime()));
                    invokeEventIfEditing();
                }
                
                break;
            }
			default:
            {
                ImGui::EndPopup();
                return;
            }
		}

        if (m_editing == true && (m_filterState.getType() != SCH_SELECT && m_filterState.getType() != SCH_DATE))
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
    m_filterState.setFilter(m_scheduleCore->getColumn(column)->filters.at(filterIndex));

    // set m_selectedDateMode based on the filter's Date pass value if it is a Date filter
    if (m_filterState.getType() == SCH_DATE)
    {
        m_selectedDateMode = m_filterState.getFilter<DateContainer>()->getPassValue().getIsRelative() ? DateMode::Relative : DateMode::Absolute;
    }

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
        case(SCH_TIME):
        {
            m_filterState.setFilter(std::make_shared<Filter<TimeContainer>>(TimeContainer(0, 0)));
            break;
        }
        case(SCH_DATE):
        {
            // default date mode to relative
            m_selectedDateMode = DateMode::Relative;
            m_filterState.setFilter(std::make_shared<Filter<DateContainer>>(DateContainer(tm(), true, 0)));
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