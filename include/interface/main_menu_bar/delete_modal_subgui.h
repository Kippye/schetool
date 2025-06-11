#pragma once

#include "gui.h"

class DeleteModalSubGui : public Gui {
    private:
        std::string m_deleteConfirmationScheduleName = "";

    public:
        DeleteModalSubGui(const char* ID) : Gui(ID) {
        }

        Event<std::string> deleteScheduleEvent;

        void draw(Window& window, Input& input, GuiTextures& guiTextures) override;
        void setAffectedScheduleName(const std::string& name);
};