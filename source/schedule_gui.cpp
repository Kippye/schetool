
#include <array>
#include <ctime>
#include <iterator>
#include <string>
#include <cstdio>
#include <regex>
#include <algorithm>
#include <schedule_constants.h>
#include <schedule_gui.h>
#include <util.h>
#include <element.h>
#include <element_base.h>
#include "filter.h"
#include <schedule.h>

#include <iostream>

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
				if (ScheduleGui::displayDateEditor(m_editorDate, m_viewedYear, m_viewedMonth))
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

        ImGui::Text("%s is ", m_scheduleCore->getColumn(m_editorColumn)->name.c_str());

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
            
                // Button displaying the date of the current Date element
                if (ImGui::Button(value.getString().append("##filterEditor").append(std::to_string(m_editorColumn)).c_str()))
                {

                }

                if (m_editing == true)
                {
                    ImGui::SameLine();
                    displayRemoveFilterButton();
                }

                if (ScheduleGui::displayDateEditor(value, m_viewedYear, m_viewedMonth))
                {
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


void ScheduleGui::setScheduleCore(const ScheduleCore& scheduleCore)
{
	m_scheduleCore = &scheduleCore;

	addSubGui("ElementEditorSubGui", new ElementEditorSubGui("ElementEditorSubGui", m_scheduleCore));
	addSubGui("FilterEditorSubGui", new FilterEditorSubGui("FilterEditorSubGui", m_scheduleCore));
}

void ScheduleGui::draw(Window& window, Input& input)
{
    const float FILTER_SPACE_VERTICAL = 0.0f; // 48.0f;
    const float SCHEDULE_OFFSET = 32.0f; // highestFilterCount * FILTER_SPACE_VERTICAL;
    const float ADD_ROW_BUTTON_HEIGHT = 32.0f;
    const float ADD_COLUMN_BUTTON_WIDTH = 32.0f;
    const float CHILD_WINDOW_WIDTH = (float)(window.SCREEN_WIDTH - ADD_COLUMN_BUTTON_WIDTH - 6);
    const float CHILD_WINDOW_HEIGHT = (float)(window.SCREEN_HEIGHT - SCHEDULE_OFFSET - ADD_ROW_BUTTON_HEIGHT - 10.0f);
    //ImGui::SetNextWindowSizeConstraints(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT), ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
	ImGui::SetNextWindowSize(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

	ImGui::Begin(m_ID.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		// TODO: For the schedule table, combine
		// Reorderable, hideable, with headers & ImGuiTableFlags_ScrollY and background colours and context menus in body and custom headers
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	    ImGui::SetNextWindowPos(ImVec2(0.0, SCHEDULE_OFFSET));
		ImGui::BeginChild("SchedulePanel", ImVec2(CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT), true);
			ImGuiTableFlags tableFlags = ImGuiTableFlags_Reorderable | ImGuiTableRowFlags_Headers | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_Borders;
			if (ImGui::BeginTable("ScheduleTable", m_scheduleCore->getColumnCount(), tableFlags))
			{ 
				for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
				{
					ImGui::TableSetupColumn(m_scheduleCore->getColumn(column)->name.c_str());
				}
 
                // filters row
                ImGui::TableNextRow();
                for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
                {
                    ImGui::TableSetColumnIndex(column);

                    if (ImGui::SmallButton(std::string("+##addFilter").append(std::to_string(column)).c_str()))
                    {
                        // display the Filter editor to add a filter to this Column
                        if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui"))
                        {
                            filterEditor->open_create(column, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                        }
                    }
                        
                    if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui"))
                    {
                        if (filterEditor->getFilterColumn() == column)
                        {
                            filterEditor->draw(window, input);
                        }
                    }

                    const Column* currentColumn = m_scheduleCore->getColumn(column);

                    ImGui::SameLine();

                    for (size_t i = 0; i < currentColumn->filters.size(); i++)
                    {
                        float filterButtonWidth = ImGui::GetColumnWidth(-1) / currentColumn->filters.size();
                        if (ImGui::Button(std::string(m_scheduleCore->getColumn(column)->name).append("##").append(std::to_string(i)).c_str(), ImVec2(filterButtonWidth, 0)))
                        {
                            if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui"))
                            {
                                filterEditor->open_edit(column, i, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                            }
                        }

                        if (i < currentColumn->filters.size() - 1)
                        {
                            ImGui::SameLine();
                        }
                    }
                }

				// custom header row
				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
				for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
				{
					ImGui::TableSetColumnIndex(column);
					const char* columnName = ImGui::TableGetColumnName(column); // get name passed to TableSetupColumn()
					ImGui::PushID(column);
					// sort button!
					if (ImGui::ArrowButton(std::string("##sortColumn").append(std::to_string(column)).c_str(), m_scheduleCore->getColumn(column)->sort == COLUMN_SORT_NONE ? ImGuiDir_Right : (m_scheduleCore->getColumn(column)->sort == COLUMN_SORT_DESCENDING ? ImGuiDir_Down : ImGuiDir_Up)))
					{
						setColumnSort.invoke(column, m_scheduleCore->getColumn(column)->sort == COLUMN_SORT_NONE ? COLUMN_SORT_DESCENDING : (m_scheduleCore->getColumn(column)->sort == COLUMN_SORT_DESCENDING ? COLUMN_SORT_ASCENDING : COLUMN_SORT_NONE));
					}
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
					// close button
					// permanent columns can't be removed so there's no need for a remove button
					if (m_scheduleCore->getColumn(column)->permanent == false)
					{
						if (ImGui::Button("X##removecolumn", ImVec2(20.0, 20.0)))
						{
							removeColumn.invoke(column);
						}
						ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
					}
					ImGui::TableHeader(columnName);
					// column header context menu
                    if (ImGui::BeginPopupContextItem("#ColumnEdit"))
                    {
						displayColumnContextPopup(column, tableFlags);
                        ImGui::EndPopup();
                    }
					ImGui::PopID();
				}

				std::vector<size_t> sortedRowIndices = m_scheduleCore->getSortedRowIndices();

				for (size_t unsortedRow = 0; unsortedRow < m_scheduleCore->getRowCount(); unsortedRow++)
				{
					size_t row = sortedRowIndices[unsortedRow];
					
                    // CHECK FILTERS BEFORE DRAWING ROW
                    for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
                    {
                        // check if the row's Element passes all Filters in this Column
                        for (const auto& filter: m_scheduleCore->getColumn(column)->filters)
                        {
                            // fails to pass, don't show this row                            
                            if (filter->checkPasses(m_scheduleCore->getElementConst(column, row)) == false)
                            {
                                goto do_not_draw_row;
                            }
                        }
                    }

					ImGui::TableNextRow();
					for (size_t column = 0; column < m_scheduleCore->getColumnCount(); column++)
					{
						ImGui::TableSetColumnIndex(column);
						// the buttons for removing rows are displayed in the first displayed column
						if (ImGui::GetCurrentTable()->Columns[column].DisplayOrder == 0)
						{
							if (ImGui::Button(std::string("X##").append(std::to_string(row)).c_str(), ImVec2(26.0, 26.0)))
							{
								removeRow.invoke(row);
								// break because this row can't be drawn anymore, it was removed.
								break;
							}
							ImGui::SameLine();
						}

						SCHEDULE_TYPE columnType = m_scheduleCore->getColumn(column)->type;
						// TODO: i could probably reduce the code repetition here
						ImGui::SetNextItemWidth(-FLT_MIN);
 
						switch(columnType)
						{
							case(SCH_BOOL):
							{
                                bool newValue = m_scheduleCore->getElementValueConstRef<bool>(column, row);
                                if (ImGui::Checkbox(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue))
                                {
                                    setElementValueBool.invoke(column, row, newValue);
                                }
								break;
							}
							case(SCH_NUMBER):
							{
                                int newValue = m_scheduleCore->getElementValueConstRef<int>(column, row);
                                if (ImGui::InputInt(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue, 0, 100, ImGuiInputTextFlags_EnterReturnsTrue))
                                {
                                    setElementValueNumber.invoke(column, row, newValue);
                                }
								break;
							}
							case(SCH_DECIMAL):
							{
                                double newValue = m_scheduleCore->getElementValueConstRef<double>(column, row);
                                if (ImGui::InputDouble(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue, 0.0, 0.0, "%.15g", ImGuiInputTextFlags_EnterReturnsTrue))
                                {
                                    setElementValueDecimal.invoke(column, row, newValue);
                                }
								break;
							}
							case(SCH_TEXT):
							{
                                std::string value = m_scheduleCore->getElementValueConstRef<std::string>(column, row);
                                value.reserve(ELEMENT_TEXT_MAX_LENGTH);
                                char* buf = value.data();
                                //ImGui::InputText(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity());
                                if (ImGui::InputTextMultiline(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), buf, value.capacity(), ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
                                {
                                    setElementValueText.invoke(column, row, std::string(buf));
                                }
                                // ImVec2 textSize = ImGui::CalcTextSize(buf);
                                // if (textSize.x == 106 && textSize.y == 16)
                                // {
                                // 	ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetItemID());
                                // 	std::cout << "newline" << std::endl;
                                // 	state->OnKeyPressed((int)'\n');
                                // 	std::cout << buf << std::endl;
                                // }
                                // std::cout << textSize.x << "; " << textSize.y << std::endl;
								break;
							}
							case(SCH_SELECT):
							{
                                SelectContainer value = m_scheduleCore->getElementValueConstRef<SelectContainer>(column, row);
                                auto selection = value.getSelection();
                                const std::vector<std::string>& optionNames = m_scheduleCore->getColumn(column)->selectOptions.getOptions();

                                std::vector<int> selectionIndices = {};

                                // error fix attempt: there should never be more selected that options
                                while (selection.size() > optionNames.size())
                                {
                                    printf("ScheduleGui::draw: Select at (%zu; %zu) has more indices in selection (%zu) than existing options (%zu). Removing selection indices until valid!\n", column, row, selection.size(), optionNames.size());
                                    selection.erase(--selection.end());
                                }

                                for (size_t s: selection)
                                {
                                    // error fix attempt: there should never be selection indices that are >= optionNames.size()
                                    if (s >= optionNames.size())
                                    {
                                        printf("ScheduleGui::draw: Select at (%zu; %zu) index (%zu) >= optionNames.size() (%zu). Removing index from selection.\n", column, row, s, optionNames.size());
                                        selection.erase(s);
                                    }
                                }

                                size_t selectedCount = selection.size();

                                for (size_t s: selection)
                                {
                                    selectionIndices.push_back(s);
                                }

                                // sort indices so that the same options are always displayed in the same order
                                std::sort(std::begin(selectionIndices), std::end(selectionIndices));

                                float displayedChars = 0;
                                float pixelsPerCharacter = 12.0f;
                                float columnWidth = ImGui::GetColumnWidth(column);

                                for (size_t i = 0; i < selectedCount; i++)
                                {
                                    // TODO: colors later ImGui::PushStyleColor(ImGuiCol_Button, m_dayColours[i]);
                                    if (ImGui::ButtonEx(std::string(optionNames[selectionIndices[i]]).append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str(), ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
                                    {
                                        // left clicking opens the editor like the user would expect
                                        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                                        {
                                            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                            {
                                                elementEditor->open(column, row, SCH_SELECT, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                                elementEditor->setEditorValue(value);
                                            }
                                        }
                                        // right clicking erases the option - bonus feature
                                        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                                        {
                                            value.setSelected(selectionIndices[i], false);
                                            setElementValueSelect.invoke(column, row, value); 
                                        }
                                    }
                                    // HACK to make this show when any of the options is hovered
                                    if (i != selectedCount - 1 && ImGui::IsItemHovered())
                                    {
                                        ImGui::BeginTooltip();
                                        ImGui::Text("Created: %s %s", m_scheduleCore->getElementConst(column, row)->getCreationDate().getString().c_str(), m_scheduleCore->getElementConst(column, row)->getCreationTime().getString().c_str());
                                        ImGui::EndTooltip();
                                    }

                                    displayedChars += optionNames[selectionIndices[i]].length();
                                    if (i < selectedCount - 1 && floor(displayedChars * pixelsPerCharacter / columnWidth) == floor((displayedChars - optionNames[selectionIndices[i]].length()) * pixelsPerCharacter / columnWidth))
                                    {
                                        ImGui::SameLine();
                                    }
                                    // ImGui::PopStyleColor(1);
                                }

                                // TEMP ? if there are no options selected, just show an "Edit" button to prevent kind of a softlock
                                if (selectedCount == 0)
                                {
                                    if (ImGui::Button(std::string("Edit##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
                                    {
                                        if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                        {
                                            elementEditor->open(column, row, SCH_SELECT, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                            elementEditor->setEditorValue(value);
                                        }
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueSelect.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
							case(SCH_TIME):
							{
                                TimeContainer value = m_scheduleCore->getElementValueConstRef<TimeContainer>(column, row);

                                // Button displaying the Time of the current Time element
                                if (ImGui::Button(value.getString().append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
                                {
                                    if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                    {
                                        elementEditor->setEditorValue(value);
                                        elementEditor->open(column, row, SCH_TIME, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueTime.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
							case(SCH_DATE):
							{
                                auto value = m_scheduleCore->getElementValueConstRef<DateContainer>(column, row);
                            
                                // Button displaying the date of the current Date element
                                if (ImGui::Button(value.getString().append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
                                {
                                    if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                    {
                                        elementEditor->setEditorValue(value);
                                        elementEditor->open(column, row, SCH_DATE, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueDate.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("Created: %s %s", m_scheduleCore->getElementConst(column, row)->getCreationDate().getString().c_str(), m_scheduleCore->getElementConst(column, row)->getCreationTime().getString().c_str());
							ImGui::EndTooltip();
						}
					}
                    // END OF for (size_t unsortedRow = 0; unsortedRow < m_scheduleCore->getRowCount(); unsortedRow++)
                    do_not_draw_row:
                    bool b = false; // stupid thing because fsr the label can't be at the end of the loop
				}
				ImGui::EndTable();
			}
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::Button("+", ImVec2(ADD_COLUMN_BUTTON_WIDTH, (float)(CHILD_WINDOW_HEIGHT))))
		{
			addDefaultColumn.invoke(m_scheduleCore->getColumnCount());
		}
		if (ImGui::Button("Add row", ImVec2((float)(window.SCREEN_WIDTH - ADD_COLUMN_BUTTON_WIDTH - 26), ADD_ROW_BUTTON_HEIGHT)))
		{
			addRow.invoke(m_scheduleCore->getRowCount());
		}
		ImGui::PopStyleVar();
    ImGui::End();
}

void ScheduleGui::displayColumnContextPopup(unsigned int column, ImGuiTableFlags tableFlags)
{
	// FIXME TODO STUPID HACK !!
	ImGuiTable* table = ImGui::GetCurrentContext()->Tables.GetByIndex(0) ;
	//ImGui::TableFindByID(ImGui::GetID("ScheduleTable"));

	// renaming
	std::string name = m_scheduleCore->getColumn(column)->name.c_str();
	name.reserve(COLUMN_NAME_MAX_LENGTH);
	char* buf = name.data();
	
	if (ImGui::InputText(std::string("##columnName").append(std::to_string(column)).c_str(), buf, name.capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		setColumnName.invoke(column, buf);
	}

	// select type (for non-permanent columns)
	if (m_scheduleCore->getColumn(column)->permanent == false)
	{
		ImGui::Separator();
		SCHEDULE_TYPE selected = m_scheduleCore->getColumn(column)->type;
		for (unsigned int i = 0; i < (unsigned int)SCH_LAST; i++)
		{
			if (ImGui::Selectable(schedule_consts::scheduleTypeNames.at((SCHEDULE_TYPE)i), selected == (SCHEDULE_TYPE)i))
				setColumnType.invoke(column, SCHEDULE_TYPE(i));
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
			if (m_scheduleCore->getColumn(otherColumnIndex)->permanent)
			{
				continue;
			}
			ImGuiTableColumn* otherColumn = &table->Columns[otherColumnIndex];
			const char* name = ImGui::TableGetColumnName(table, otherColumnIndex);
			if (name == NULL || name[0] == 0)
				name = "Unnamed";

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
}

bool ScheduleGui::displayDateEditor(DateContainer& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth)
{
    bool changedDate = false;
    ImGui::Text("%s", editorDate.getString().c_str());
    // DATE / CALENDAR
    if (ImGui::ArrowButton("##PreviousMonth", ImGuiDir_Left))
    {
        viewedMonth = viewedMonth == 0 ? 11 : viewedMonth - 1;
    }
    ImGui::SameLine();
    char monthName[16];
    tm formatTime;
    formatTime.tm_mon = viewedMonth;
    std::strftime(monthName, sizeof(monthName), "%B", &formatTime);
    ImGui::Button(std::string(monthName).append(std::string("##Month")).c_str(), ImVec2(96, 0));
    ImGui::SameLine();
    if (ImGui::ArrowButton("##NextMonth", ImGuiDir_Right))
    {
        viewedMonth = viewedMonth == 11 ? 0 : viewedMonth + 1;
    }
    int yearInput = viewedYear + 1900;
    if (ImGui::InputInt("##YearInput", &yearInput, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        viewedYear = DateContainer::convertToValidYear(yearInput);
    }

    unsigned int daysInMonth = mytime::get_month_day_count(viewedYear, viewedMonth);

    for (unsigned int day = 1; day <= daysInMonth; day++)
    {
        if (ImGui::Button(std::to_string(day).c_str(), ImVec2(24, 24)))
        {
            editorDate.setYear(viewedYear, false);
            editorDate.setMonth(viewedMonth);
            editorDate.setMonthDay(day); 
            changedDate = true;
        }
        if (day == editorDate.getTime().tm_mday)
        {
            // TODO: Highlight this day as selected in the calendar
        }
        // sameline when not the last day of the week and not the last day of the month (trailing sameline = bad)
        if (day % 7 != 0 && day < daysInMonth)
        {
            ImGui::SameLine();
        }
    }
    return changedDate;
}