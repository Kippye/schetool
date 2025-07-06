#pragma once

#include <vector>
#include "event.h"
#include "event_pipe.h"
#include "gui.h"
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

        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
        void passFileNames(const std::vector<std::string>& fileNames);
};