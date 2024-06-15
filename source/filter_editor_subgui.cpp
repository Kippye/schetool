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
                bool newValue = m_filterState.getFilter<bool>()->getPassValue();
                if (ImGui::Checkbox(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterState.getFilter<bool>()->setPassValue(newValue);
                }
                break;
            }
            case(SCH_NUMBER):
            {
                int newValue = m_filterState.getFilter<int>()->getPassValue();
                if (ImGui::InputInt(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterState.getFilter<int>()->setPassValue(newValue);
                }
                break;
            }
            case(SCH_DECIMAL):
            {
                double newValue = m_filterState.getFilter<double>()->getPassValue();
                if (ImGui::InputDouble(std::string("##filterEditor").append(std::to_string(m_editorColumn)).append(";").c_str(), &newValue))
                {
                    m_filterState.getFilter<double>()->setPassValue(newValue);
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
                }

                break;
            }
            // case(SCH_SELECT):
            // {

                // break;
            // }
            case(SCH_TIME):
            {
                TimeContainer value = m_filterState.getFilter<TimeContainer>()->getPassValue();

                // Button displaying the Time of the current Time element
                if (ImGui::Button(value.getString().append("##filterEditor").append(std::to_string(m_editorColumn)).c_str()))
                {
                    // TODO: OPEN EDITOR TO EDIT VALUE
                    // if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                    // {
                    //     elementEditor->setEditorValue(value);
                    //     elementEditor->open(column, row, SCH_TIME, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                    // }
                }
                // if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                // {
                //     auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                //     if (column == editorColumn && row == editorRow)
                //     {
                //         elementEditor->draw(window, input);
                //         // was editing this Element, made edits and just closed the editor. apply the edits
                //         if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                //         {
                //             setElementValueTime.invoke(column, row, elementEditor->getEditorValue(value));
                //         }
                //     }
                // }
 
                break;
            }
            case(SCH_DATE):
            {
                DateContainer value = m_filterState.getFilter<DateContainer>()->getPassValue();
            
                // Switch between a relative and absolute filter
                if (ImGui::Combo("##filterEditorDateMode", (int*)&m_selectedDateMode, m_dateModeOptions))
                {
                    if (m_selectedDateMode == DateMode::Absolute)
                    {
                        m_filterState.getFilter<DateContainer>()->setPassValue(DateContainer(value.getTime(), false));
                    }
                    else
                    {
                        m_filterState.getFilter<DateContainer>()->setPassValue(DateContainer(value.getTime(), true));
                    }
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

                if (gui_templates::DateEditor(value, m_viewedYear, m_viewedMonth))
                {
                    m_selectedDateMode = DateMode::Absolute;
                    // weird lil thing: if a Date was selected, then the Date is no longer relative. so we make a copy using its time but with relative = false
                    m_filterState.getFilter<DateContainer>()->setPassValue(DateContainer(value.getTime()));
                }
                
                break;
            }
			default:
            {
                ImGui::EndPopup();
                return;
            }
		}

        if (m_editing == true && m_filterState.getType() != SCH_DATE)
        {
            ImGui::SameLine();
            displayRemoveFilterButton();
        }

        if (ImGui::Button(m_editing == true ? "Apply" : "Create"))
        {
            if (m_editing)
            {
                editColumnFilter.invoke(m_editorColumn, m_editorFilterIndex, m_filterState.getFilterBase());
            }
            else
            {
                addColumnFilter.invoke(m_editorColumn, m_filterState.getFilterBase());
            }
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
        // case(SCH_SELECT):
        // {
        //     Filter filter = Filter(SelectContainer());
        //     m_filterState.setFilter(std::make_shared(&filter));
        //     break;
        // }
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