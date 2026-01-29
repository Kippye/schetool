#pragma once

#include <vector>
#include "event.h"
#include "event_pipe.h"
#include "gui.h"
#include "file_info.h"

class StartPageGui : public Gui {
    private:
        std::vector<FileInfo> m_fileInfoList = {};
        bool m_openScheduleNameModal = false;

    public:
        StartPageGui(const char* ID);

        Event<FileInfo> openScheduleFileEvent;
        // Event pipes
        EventPipe<std::string> createNewScheduleEventPipe;

        void draw(GuiDrawArgs& args) override;
        void passFileInfoList(const std::vector<FileInfo>& fileInfoList);
};