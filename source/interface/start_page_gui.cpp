#include "start_page_gui.h"
#include "text_input_modal_subgui.h"

StartPageGui::StartPageGui(const char* ID) : Gui(ID) {
    setVisible(false);
    addSubGui(new TextInputModalSubGui("StartPageNewNameModalSubGui", "Enter name   ", "Create schedule"));

    createNewScheduleEventPipe.addEvent(
        getSubGui<TextInputModalSubGui>("StartPageNewNameModalSubGui")->acceptButtonPressedEvent);
}

void StartPageGui::draw(GuiDrawArgs& args) {
    if (m_visible == false) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2((float)args.windowSize.getWidth(), (float)args.windowSize.getHeight()));
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

    ImGui::Begin(m_ID.c_str(),
                 NULL,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::NewLine();
    ImFont* pageTitleFont = InterfaceStyleHandler::getFontData(InterfaceStyleHandler::getFontSize() == FontSize::Large
                                                                   ? FontSize::Large
                                                                   : (FontSize)((int)InterfaceStyleHandler::getFontSize() + 1));

    ImGui::PushFont(pageTitleFont);
    const float iconSize = ImGui::CalcTextSize("Schetool").y * 2.0f;
    ImGui::Image(args.guiTextures.getOrLoad("icon").ImID, ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(
        ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y / 4.0f));
    ImGui::Text("Schetool");
    ImGui::PopFont();
    ImGui::Text("Create a new file");
    if (ImGui::Button("New##StartPageGuiNewButton")) {
        m_openScheduleNameModal = true;
    }
    ImGui::Text("Open a file");
    for (size_t i = 0; i < m_fileInfoList.size(); i++) {
        if (ImGui::Button(m_fileInfoList[i].getStem().append("##StartPageGuiFileListButton").c_str())) {
            openScheduleFileEvent.invoke(m_fileInfoList[i]);
        }
    }
    ImGui::End();

    if (auto nameModalSubGui = getSubGui<TextInputModalSubGui>("StartPageNewNameModalSubGui")) {
        nameModalSubGui->draw(args);

        if (m_openScheduleNameModal) {
            nameModalSubGui->open();
            m_openScheduleNameModal = false;
        }
    }
}

void StartPageGui::passFileInfoList(const std::vector<FileInfo>& fileInfoList) {
    m_fileInfoList = fileInfoList;
}