#include "start_page_gui.h"

StartPageGui::StartPageGui(const char* ID) : Gui(ID)
{
    setVisible(false);
}

void StartPageGui::draw(Window& window, Input& input)
{
    if (m_visible == false) { return; }

    ImGui::Begin("Welcome to Brazil!", NULL);

    ImGui::End();
}