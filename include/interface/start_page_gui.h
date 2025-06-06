#pragma once

#include <vector>
#include "event.h"
#include "event_pipe.h"
#include "gui.h"
#include "window.h"
#include "input.h"

class StartPageGui : public Gui {
    private:
        std::vector<std::string> m_fileNames = {};
        bool m_openScheduleNameModal = false;

    public:
        StartPageGui(const char* ID);

        Event<std::string> openScheduleFileEvent;
        // Event pipes
        EventPipe<std::string> createNewScheduleEventPipe;

        void draw(Window& window, Input& input, GuiTextures& guiTextures) override;
        void passFileNames(const std::vector<std::string>& fileNames);
};