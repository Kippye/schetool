
#include <array>
#include <iterator>
#include <string>
#include <cstdio>
#include <algorithm>
#include <format>
#include "schedule_constants.h"
#include "schedule_gui.h"
#include "element_editor_subgui.h"
#include "filter_editor_subgui.h"
#include "gui_templates.h"
#include "gui_constants.h"

ScheduleGui::ScheduleGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents, const std::shared_ptr<const MainMenuBarGui> mainMenuBarGui) : m_scheduleCore(scheduleCore), Gui(ID), m_mainMenuBarGui(mainMenuBarGui)
{
    // TEMP ?
    m_font32x = ImGui::GetIO().Fonts->AddFontFromFileTTF("./fonts/Noto_Sans_Mono/NotoSansMono-VariableFont.ttf", 32.0f);

	addSubGui(new ElementEditorSubGui("ElementEditorSubGui", m_scheduleCore));
	addSubGui(new FilterEditorSubGui("FilterEditorSubGui", m_scheduleCore, scheduleEvents));
}

void ScheduleGui::draw(Window& window, Input& input, GuiTextures& guiTextures)
{
    if (m_visible == false) { return; }

    ImGuiStyle style = ImGui::GetStyle();
    ImVec2 labelSize = ImGui::CalcTextSize("M", NULL, true);
    float resetButtonSize = ImGui::CalcItemSize(ImVec2(0, 0), labelSize.x + style.ItemInnerSpacing.x * 2.0f, labelSize.y + style.ItemInnerSpacing.y * 2.0f).y;
    const float SCHEDULE_TOP_BAR_HEIGHT = resetButtonSize + style.ItemSpacing.y * 2;
    const float ADD_ROW_BUTTON_HEIGHT = 32.0f;
    const float ADD_COLUMN_BUTTON_WIDTH = 32.0f;
    const float SCHEDULE_OFFSET = SCHEDULE_TOP_BAR_HEIGHT + 32.0f;
    const float CHILD_WINDOW_WIDTH = (float)(window.SCREEN_WIDTH - ADD_COLUMN_BUTTON_WIDTH - 8);
    const float CHILD_WINDOW_HEIGHT = (float)(window.SCREEN_HEIGHT - SCHEDULE_OFFSET - ADD_ROW_BUTTON_HEIGHT - 16.0f);
    //ImGui::SetNextWindowSizeConstraints(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT), ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
	ImGui::SetNextWindowSize(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
	ImGui::SetNextWindowContentSize(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT) - style.WindowPadding * 2.0f);
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

	ImGui::Begin(m_ID.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
        // Add menu bar height as offset
        if (m_mainMenuBarGui)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_mainMenuBarGui->getHeight());
        }
        if (ImGui::ImageButton("##ResetToTodayButton", (ImTextureID)guiTextures.getOrLoad("icon_reset"), ImVec2(resetButtonSize, resetButtonSize) - style.FramePadding * 2.0f))
        {
            m_scheduleDateOverride.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("View specific date"))
        {
            m_openDateSelectPopup = true;
        }
        if (ImGui::BeginPopup("Schedule Date Selector"))
        {
            // Display date editor to edit m_scheduleDateOverride.
            // If the current date was selected, just clear m_scheduleDateOverride again.
            if (gui_templates::DateEditor(m_scheduleDateOverride, m_dateSelectorYear, m_dateSelectorMonth) && m_scheduleDateOverride.getDateUTC() == TimeWrapper::getCurrentTime().getDateUTC())
            {
                m_scheduleDateOverride.clear();
            }
            ImGui::EndPopup();
        }
        if (m_openDateSelectPopup)
        {
            TimeWrapper currentTime = TimeWrapper::getCurrentTime();
            m_dateSelectorYear = currentTime.getYearUTC();
            m_dateSelectorMonth = currentTime.getMonthUTC();
            ImGui::OpenPopup("Schedule Date Selector");
            m_openDateSelectPopup = false;
        }
        ImGui::SameLine();
        // Current date text
        const TimeWrapper& currentDate = m_scheduleDateOverride.getIsEmpty() == false ? m_scheduleDateOverride : TimeWrapper::getCurrentTime();
        const std::string_view currentDateFmt = currentDate.getMonthDayUTC() < 10 ? "{:%A,%e. %B %Y}" : "{:%A, %e. %B %Y}";
        std::string viewedDateText = m_scheduleDateOverride.getIsEmpty() == true ? currentDate.getDynamicFmtString(currentDateFmt) : currentDate.getDynamicFmtStringUTC(currentDateFmt);
        // std::string viewedDateText = m_scheduleDateOverride.getIsEmpty() == true ? TimeWrapper::getCurrentTime().getString(TIME_FORMAT_DATE) : m_scheduleDateOverride.getStringUTC(TIME_FORMAT_DATE);
        ImGui::PushFont(m_font32x);
        const float viewedDateTextWidth = ImGui::CalcTextSize(viewedDateText.c_str()).x;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - viewedDateTextWidth / 2.0f);
        ImGui::Text("%s", viewedDateText.c_str());
        ImGui::PopFont();
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	    ImGui::SetNextWindowPos(ImVec2(0.0, SCHEDULE_OFFSET));
		ImGui::BeginChild("SchedulePanel", ImVec2(CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT), true);
			ImGuiTableFlags tableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_ScrollX;
            // avoid imgui 0 column abort by not beginning the table at all if there are no columns in the schedule
            if (m_scheduleCore.getColumnCount() == 0)
            {
                goto skip_schedule_table;
            }
			if (ImGui::BeginTable("ScheduleTable", m_scheduleCore.getColumnCount(), tableFlags, ImGui::GetContentRegionAvail()))
			{
				for (size_t column = 0; column < m_scheduleCore.getColumnCount(); column++)
				{
					ImGui::TableSetupColumn(m_scheduleCore.getColumn(column)->name.c_str());
				}
 
                // filters row
                ImGui::TableNextRow();
                for (size_t column = 0; column < m_scheduleCore.getColumnCount(); column++)
                {
                    ImGui::TableSetColumnIndex(column);

                    const ImVec2 label_size = ImGui::CalcTextSize("W", NULL, true);
                    float addFilterButtonSize = ImGui::CalcItemSize(ImVec2(0.0f, 0.0f), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f).y;
                    if (ImGui::Button(std::format("+##addFilterGroup{}", column).c_str(), ImVec2(addFilterButtonSize, addFilterButtonSize)))
                    {
                        // display the FilterGroup editor to add a filter group to this Column
                        if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui"))
                        {
                            filterEditor->createGroupAndEdit(column, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                        }
                    }
                        
                    if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui"))
                    {
                        if (filterEditor->getColumn() == column)
                        {
                            filterEditor->draw(window, input, guiTextures);
                        }
                    }

                    ImGui::SameLine();

                    const Column* currentColumn = m_scheduleCore.getColumn(column);
                    const auto& columnFilterGroups = currentColumn->getFilterGroupsConst();

                    // DATA TO PASS TO FILTER EDITOR
                    bool openFilterEditor = false;
                    size_t editorGroupIndex = 0;
                    ImRect itemAvoidRect;

                    // LAMBDA: Draws buttons for every FilterGroup in the column. Sets data to pass to filterEditor if a button is clicked.
                    auto drawFilterGroupButtons = [&](bool sameLine, float buttonWidth)
                    {
                        for (size_t i = 0; i < columnFilterGroups.size(); i++)
                        {
                            const auto& filterGroup = currentColumn->getFilterGroupConst(i);
                            if (filterGroup.getIsEnabled() == false)
                            {
                                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::inactiveAlpha);
                            }
                            // FilterGroup button with its name
                            if (ImGui::Button(filterGroup.getName().append("##").append(std::to_string(column)).append(";").append(std::to_string(i)).c_str(), ImVec2(buttonWidth, 0)))
                            {
                                openFilterEditor = true;
                                editorGroupIndex = i;
                                itemAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                            }
                            if (filterGroup.getIsEnabled() == false)
                            {
                                ImGui::PopStyleVar();
                            }

                            if (sameLine && i < currentColumn->getFilterGroupCount() - 1)
                            {
                                ImGui::SameLine();
                            }
                        }
                    };

                    const float filterButtonWidth = (ImGui::GetColumnWidth() - (ImGui::GetStyle().ItemSpacing.x * columnFilterGroups.size() - 1))  / columnFilterGroups.size();

                    if (columnFilterGroups.size() <= 3)
                    {
                        drawFilterGroupButtons(true, filterButtonWidth);
                    }
                    else
                    {
                        if (ImGui::Button(std::format("{} filter groups##OpenFilterGroupListButton{}", columnFilterGroups.size(), column).c_str()))
                        {
                            m_filterGroupListColumn = column;
                            ImGui::OpenPopup("FilterGroupListPopup");
                        }
                        if (m_filterGroupListColumn == column && ImGui::BeginPopup("FilterGroupListPopup"))
                        {
                            drawFilterGroupButtons(false, ImGui::CalcTextSize("M").x * filter_consts::FILTER_GROUP_NAME_MAX_LENGTH);
                            ImGui::EndPopup();
                        }
                    }
                    // Open filter editor NOTE: All the data here is set in drawFilterGroupButtons() lambda!
                    if (openFilterEditor)
                    {
                        if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui"))
                        {
                            filterEditor->openGroupEdit(column, editorGroupIndex, itemAvoidRect);
                        }
                    }
                }

				// custom header row
				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
				for (size_t column = 0; column < m_scheduleCore.getColumnCount(); column++)
				{
					ImGui::TableSetColumnIndex(column);
					const char* columnName = ImGui::TableGetColumnName(column); // get name passed to TableSetupColumn()
                    bool isColumnHeaderHovered = (ImGui::TableGetHoveredColumn() == column && ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex());
					ImGui::PushID(column);
                    float headerCursorY = ImGui::GetCursorPosY();
                    size_t pushedStyleVars = 0;
					// HIDE the sort button if the column header is not hovered and the column does not have a sort direction applied
                    if (isColumnHeaderHovered == false && m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_NONE)
					{
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);
                        pushedStyleVars++;
					}
                    // sort button!
                    if (ImGui::ArrowButton(std::string("##sortColumn").append(std::to_string(column)).c_str(), m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_NONE ? ImGuiDir_Right : (m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_DESCENDING ? ImGuiDir_Down : ImGuiDir_Up)))
                    {
                        setColumnSort.invoke(column, m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_NONE ? COLUMN_SORT_DESCENDING : (m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_DESCENDING ? COLUMN_SORT_ASCENDING : COLUMN_SORT_NONE));
                    }
                    ImGui::PopStyleVar(pushedStyleVars);
                    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
					ImGui::TableHeader(columnName);
					// column header context menu
                    if (ImGui::BeginPopupContextItem("#ColumnEdit"))
                    {
						displayColumnContextPopup(column, tableFlags);
                        ImGui::EndPopup();
                    }
					// Show a close button on the right when hovered
					// permanent columns can't be removed so there's no need for a remove button
					if (isColumnHeaderHovered && m_scheduleCore.getColumn(column)->permanent == false)
					{
                        // This is how the arrow button's size is calculated
                        float headerButtonSize = ImGui::GetFrameHeight();
                        // SameLine() can't be used after a TableHeader so the position has to be calculated manually.
                        ImGui::SetCursorPosX(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), column).Max.x - headerButtonSize - 12.0f);
                        ImGui::SetCursorPosY(headerCursorY);
                        size_t pushedColorCount = 0;
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); pushedColorCount++;
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f)); pushedColorCount++;
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.4f)); pushedColorCount++;
                        if (ImGui::ImageButtonEx(ImGui::GetID("##RemoveColumn"), (ImTextureID)guiTextures.getOrLoad("icon_remove"), ImVec2(headerButtonSize, headerButtonSize) - style.FramePadding * 2.0f, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f)))
						{
							removeColumn.invoke(column);
                            ImGui::PopStyleColor(pushedColorCount);
                            ImGui::PopID();
                            ImGui::EndTable();
                            goto skip_schedule_table;
						}
                        ImGui::PopStyleColor(pushedColorCount);
					}
					ImGui::PopID();
				}

				std::vector<size_t> sortedRowIndices = m_scheduleCore.getSortedRowIndices();

				for (size_t unsortedRow = 0; unsortedRow < m_scheduleCore.getRowCount(); unsortedRow++)
				{
					size_t row = sortedRowIndices[unsortedRow];
					
                    // CHECK FILTERS BEFORE DRAWING ROW
                    for (size_t column = 0; column < m_scheduleCore.getColumnCount(); column++)
                    {
                        // Check if the row's Element passes every FilterGroup in this Column
                        bool passesAllFilters = m_scheduleCore.getColumn(column)->checkElementPassesFilters(row, 
                            m_scheduleDateOverride // Pass override date as current (Uses TimeWrapper::getCurrentTime() if it's empty)
                        );  
                        // fails to pass, don't show this row                            
                        if (passesAllFilters == false)
                        {
                            goto do_not_draw_row;
                        }
                    }

					ImGui::TableNextRow();
					for (size_t column = 0; column < m_scheduleCore.getColumnCount(); column++)
					{
                        bool columnEditDisabled = false;
                        // If viewing a different date and the column has a reset option then show it disabled 
                        if (m_scheduleDateOverride.getIsEmpty() == false && m_scheduleCore.getColumn(column)->resetOption != ColumnResetOption::Never)
                        {
                            columnEditDisabled = true;
                            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::disabledAlpha);
                        }

						ImGui::TableSetColumnIndex(column);
						// the buttons for removing rows are displayed in the first displayed column
						if (ImGui::GetCurrentTable()->Columns[column].DisplayOrder == 0)
						{
                            size_t pushedStyleVars = 0;
                            // HIDE the row remove button unless the row is hovered
                            if (ImGui::TableGetHoveredRow() != ImGui::TableGetRowIndex())
                            {
                                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);
                                pushedStyleVars++;
                            }
							if (ImGui::Button(std::string("X##").append(std::to_string(row)).c_str(), ImVec2(26.0, 26.0)))
							{
								removeRow.invoke(row);
								// break because this row can't be drawn anymore, it was removed.
								break;
							}
                            ImGui::PopStyleVar(pushedStyleVars);
							ImGui::SameLine();
						}

						SCHEDULE_TYPE columnType = m_scheduleCore.getColumn(column)->type;
						// TODO: i could probably reduce the code repetition here
						ImGui::SetNextItemWidth(-FLT_MIN);
 
						switch(columnType)
						{
							case(SCH_BOOL):
							{
                                bool newValue = getElementValue<bool>(column, row, columnEditDisabled);
                                if (ImGui::Checkbox(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue))
                                {
                                    setElementValueBool.invoke(column, row, newValue);
                                }
								break;
							}
							case(SCH_NUMBER):
							{
                                int newValue = getElementValue<int>(column, row, columnEditDisabled);
                                if (ImGui::InputInt(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue, 0, 100, ImGuiInputTextFlags_EnterReturnsTrue))
                                {
                                    setElementValueNumber.invoke(column, row, newValue);
                                }
								break;
							}
							case(SCH_DECIMAL):
							{
                                double newValue = getElementValue<double>(column, row, columnEditDisabled);
                                if (ImGui::InputDouble(std::string("##").append(std::to_string(column)).append(";").append(std::to_string(row)).c_str(), &newValue, 0.0, 0.0, "%.15g", ImGuiInputTextFlags_EnterReturnsTrue))
                                {
                                    setElementValueDecimal.invoke(column, row, newValue);
                                }
								break;
							}
							case(SCH_TEXT):
							{
                                std::string value = getElementValue<std::string>(column, row, columnEditDisabled);
                                std::string displayedValue = value;

                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (elementEditor->getOpenThisFrame() && (column == editorColumn && row == editorRow))
                                    {
                                        // if editing this text element, use this TextWrapped as a preview, the value will actually only be applied if the editor's input is applied
                                        displayedValue = elementEditor->getEditorValue(displayedValue);
                                    }
                                }
                                // element to display the value as a wrapped, multiline text
                                ImGui::TextWrapped("%s", displayedValue.c_str());
                                // Open text editor if clicked while hovering the current column & row
                                if (columnEditDisabled == false && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::TableGetHoveredColumn() == column && ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex() && ImGui::IsAnyItemHovered() == false)
                                {
                                    if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                    {
                                        elementEditor->setEditorValue(value);
                                        elementEditor->setTextInputBoxSize(ImVec2(ImGui::GetColumnWidth(column), 0));
                                        elementEditor->open(column, row, SCH_TEXT, ImRect(ImGui::GetItemRectMin(), ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), column).Max));
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input, guiTextures);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueText.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
							case(SCH_SELECT):
							{
                                SelectContainer value = getElementValue<SelectContainer>(column, row, columnEditDisabled);
                                auto selection = value.getSelection();
                                const std::vector<SelectOption>& options = m_scheduleCore.getColumn(column)->selectOptions.getOptions();

                                std::vector<int> selectionIndices = {};

                                // error fix attempt: there should never be more selected that options
                                while (selection.size() > options.size())
                                {
                                    printf("ScheduleGui::draw: Select at (%zu; %zu) has more indices in selection (%zu) than existing options (%zu). Removing selection indices until valid!\n", column, row, selection.size(), options.size());
                                    selection.erase(--selection.end());
                                }

                                for (size_t s: selection)
                                {
                                    // error fix attempt: there should never be selection indices that are >= optionNames.size()
                                    if (s >= options.size())
                                    {
                                        printf("ScheduleGui::draw: Select at (%zu; %zu) index (%zu) >= optionNames.size() (%zu). Removing index from selection.\n", column, row, s, options.size());
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
                                    if (gui_templates::SelectOptionButton(options[selectionIndices[i]], std::format("##{};{}", column, row).c_str(), ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
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
                                        ImGui::Text("Created: %s", m_scheduleCore.getElementConst(column, row)->getCreationTime().getString().c_str());
                                        ImGui::EndTooltip();
                                    }

                                    displayedChars += options[selectionIndices[i]].name.length();
                                    if (i < selectedCount - 1 && floor(displayedChars * pixelsPerCharacter / columnWidth) == floor((displayedChars - options[selectionIndices[i]].name.length()) * pixelsPerCharacter / columnWidth))
                                    {
                                        ImGui::SameLine();
                                    }
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
                                        elementEditor->draw(window, input, guiTextures);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueSelect.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
                            case(SCH_WEEKDAY):
							{
                                WeekdayContainer value = getElementValue<WeekdayContainer>(column, row, columnEditDisabled);
                                auto selection = value.getSelection();
                                const std::vector<std::string>& optionNames = general_consts::weekdayNames;

                                std::vector<int> selectionIndices = {};

                                // error fix attempt: there should never be more selected that options
                                while (selection.size() > 7)
                                {
                                    printf("ScheduleGui::draw: WeekdayContainer at (%zu; %zu) has more indices in selection (%zu) than existing options (%d). Removing selection indices until valid!\n", column, row, selection.size(), 7);
                                    selection.erase(--selection.end());
                                }

                                for (size_t s: selection)
                                {
                                    // error fix attempt: there should never be selection indices that are >= optionNames.size()
                                    if (s >= 7)
                                    {
                                        printf("ScheduleGui::draw: WeekdayContainer at (%zu; %zu) index (%zu) >= optionNames.size() (%d). Removing index from selection.\n", column, row, s, 7);
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
                                    if (gui_templates::SelectOptionButton(SelectOption{optionNames[selectionIndices[i]], gui_colors::dayColors[selectionIndices[i]]}, std::format("##{};{}", column, row).c_str(), ImVec2(0, 0), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
                                    {
                                        // left clicking opens the editor like the user would expect
                                        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                                        {
                                            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                            {
                                                elementEditor->open(column, row, SCH_WEEKDAY, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
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
                                        ImGui::Text("Created: %s", m_scheduleCore.getElementConst(column, row)->getCreationTime().getString().c_str());
                                        ImGui::EndTooltip();
                                    }

                                    displayedChars += optionNames[selectionIndices[i]].length();
                                    if (i < selectedCount - 1 && floor(displayedChars * pixelsPerCharacter / columnWidth) == floor((displayedChars - optionNames[selectionIndices[i]].length()) * pixelsPerCharacter / columnWidth))
                                    {
                                        ImGui::SameLine();
                                    }
                                }

                                // TEMP ? if there are no options selected, just show an "Edit" button to prevent kind of a softlock
                                if (selectedCount == 0)
                                {
                                    if (ImGui::Button(std::string("Edit##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str()))
                                    {
                                        if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                        {
                                            elementEditor->open(column, row, SCH_WEEKDAY, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                                            elementEditor->setEditorValue(value);
                                        }
                                    }
                                }
                                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
                                {
                                    auto [editorColumn, editorRow] = elementEditor->getCoordinates();
                                    if (column == editorColumn && row == editorRow)
                                    {
                                        elementEditor->draw(window, input, guiTextures);
                                        // was editing this Element, made edits and just closed the editor. apply the edits
                                        if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false && elementEditor->getMadeEdits())
                                        {
                                            setElementValueWeekday.invoke(column, row, elementEditor->getEditorValue(value));
                                        }
                                    }
                                }
								break;
							}
							case(SCH_TIME):
							{
                                TimeContainer value = getElementValue<TimeContainer>(column, row, columnEditDisabled);

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
                                        elementEditor->draw(window, input, guiTextures);
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
                                auto value = getElementValue<DateContainer>(column, row, columnEditDisabled);
                            
                                // Button displaying the date of the current Date element
                                if (ImGui::Button(value.getString().append("##").append(std::to_string(column).append(";").append(std::to_string(row))).c_str(),
                                    value.getIsEmpty() ? gui_sizes::emptyLabelSize : ImVec2(0, 0))) // Set minimum width for empty Date buttons 
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
                                        elementEditor->draw(window, input, guiTextures);
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
                        if (columnEditDisabled)
                        {
                            ImGui::PopItemFlag();
                            ImGui::PopStyleVar();
                        }

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("Created: %s", m_scheduleCore.getElementConst(column, row)->getCreationTime().getString().c_str());
							ImGui::EndTooltip();
						}
					}
                    // END OF for (size_t unsortedRow = 0; unsortedRow < m_scheduleCore.getRowCount(); unsortedRow++)
                    do_not_draw_row:
                    bool b = false; // stupid thing because fsr the label can't be at the end of the loop
				}
				ImGui::EndTable();
			}
        skip_schedule_table:
		ImGui::EndChild();
		ImGui::SameLine();
        bool addColumnButtonDisabled = false;
        if (m_scheduleCore.getColumnCount() >= schedule_consts::COLUMN_MAX_COUNT)
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::disabledAlpha);
            addColumnButtonDisabled = true;
        }
		if (ImGui::Button("+", ImVec2(ADD_COLUMN_BUTTON_WIDTH, (float)(CHILD_WINDOW_HEIGHT))))
		{
			addDefaultColumn.invoke(m_scheduleCore.getColumnCount());
		}
        if (addColumnButtonDisabled)
        {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
            if (ImGui::BeginItemTooltip())
            {
                ImGui::Text("Adding too many columns is not good for you!");
                ImGui::EndTooltip();
            }
        }
		if (ImGui::Button("Add row", ImVec2((float)(window.SCREEN_WIDTH - ADD_COLUMN_BUTTON_WIDTH - 26), ADD_ROW_BUTTON_HEIGHT)))
		{
			addRow.invoke(m_scheduleCore.getRowCount());
		}
		ImGui::PopStyleVar();
    ImGui::End();
}

void ScheduleGui::displayColumnContextPopup(unsigned int columnIndex, ImGuiTableFlags tableFlags)
{
	// FIXME TODO STUPID HACK !!
	ImGuiTable* table = ImGui::GetCurrentContext()->Tables.GetByIndex(0) ;
	//ImGui::TableFindByID(ImGui::GetID("ScheduleTable"));

    const Column& column = *m_scheduleCore.getColumn(columnIndex);

	// renaming
	std::string name = column.name.c_str();
	name.reserve(COLUMN_NAME_MAX_LENGTH);
	char* buf = name.data();
	
	if (ImGui::InputText(std::string("##columnName").append(std::to_string(columnIndex)).c_str(), buf, name.capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		setColumnName.invoke(columnIndex, buf);
	}

	// select type (for non-permanent columns)
	if (column.permanent == false)
	{
		ImGui::Separator();
		SCHEDULE_TYPE selected = column.type;
		for (unsigned int i = 0; i < (unsigned int)SCH_LAST; i++)
		{
			if (ImGui::Selectable(schedule_consts::scheduleTypeNames.at((SCHEDULE_TYPE)i), selected == (SCHEDULE_TYPE)i))
				setColumnType.invoke(columnIndex, SCHEDULE_TYPE(i));
		}
	}

	ImGui::Separator(); 
	
    // Reset values
    if (ImGui::MenuItem("Reset default values", NULL, false))
    {
        resetColumn.invoke(columnIndex, true);
    }

    // Reset setting dropdown
    ImGui::Text("Reset column:");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##ColumnResetSetting", schedule_consts::columnResetOptionStrings.at(column.resetOption)))
    {
        for (auto [resetOption, settingString] : schedule_consts::columnResetOptionStrings)
        {
            bool isSelected = column.resetOption == resetOption;
            if (ImGui::Selectable(std::string(settingString).append("##").append(std::to_string((int)resetOption)).c_str(), isSelected))
            {
                setColumnResetOption.invoke(columnIndex, resetOption);
            }
        }
        ImGui::EndCombo();
    }

	// resizing
	if (tableFlags & ImGuiTableFlags_Resizable)
	{
		if (ImGui::MenuItem("Size column to fit###SizeOne", NULL, false))
			ImGui::TableSetColumnWidthAutoSingle(table, columnIndex);

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
			if (column.permanent)
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

void ScheduleGui::clearDateOverride()
{
    m_scheduleDateOverride.clear();
}