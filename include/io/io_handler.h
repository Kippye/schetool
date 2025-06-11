#pragma once

#include "schedule_io.h"
#include "preferences_io.h"
#include "file_info.h"
#include "schedule.h"
#include "window.h"
#include "input.h"
#include "interface.h"
#include <filesystem>
#include <memory>
#include <functional>
#include <string>

const unsigned int AUTOSAVE_DELAY_SECONDS = 1 * 60;

class IO_Handler {
    private:
        Schedule* m_schedule = NULL;
        Window* m_windowManager = NULL;
        std::shared_ptr<MainMenuBarGui> m_mainMenuBarGui = NULL;
        std::shared_ptr<ScheduleIO> m_scheduleIO = nullptr;
        std::shared_ptr<PreferencesIO> m_preferencesIO = nullptr;
        double m_timeSinceAutosave = 0.0;

        std::function<void(FileInfo)> openFileInfoChangeListener = [&](FileInfo fileInfo) {
            m_windowManager->setTitleSuffix(std::string(" - ").append(fileInfo.getName()));
            m_schedule->setName(fileInfo.getName());
            m_mainMenuBarGui->passOpenFileName(fileInfo.empty() ? std::nullopt
                                                                : std::optional<std::string>(fileInfo.getName()));
        };

        // input listeners
        std::function<void()> saveInputListener = std::function<void()>([&]() {
            if (m_scheduleIO) {
                FileInfo currentFileInfo = m_scheduleIO->getCurrentFileInfo();
                if (currentFileInfo.empty()) {
                    return;
                }
                m_scheduleIO->writeSchedule(currentFileInfo.getName().c_str());
            }
        });
        // window event listeners
        std::function<void()> windowCloseListener = std::function<void()>([&]() {
            if (m_scheduleIO) {
                m_scheduleIO->closeCurrentFile();
            }
        });

        std::function<void(Preferences)> preferencesModifiedListener = [&](Preferences preferences) {
            if (m_preferencesIO) {
                m_preferencesIO->setPreferences(preferences);
            }
        };

        std::function<void(Preferences)> preferencesLoadedListener = [&](Preferences preferences) {
            m_mainMenuBarGui->passPreferences(preferences);
        };

        // WINDOWS ONLY. Chooses the most suitable data directory based on the current platform and existence of certain directories
        // The data dir is the base directory onto which subdirectories will be added.
        // Usually this is either the application's install directory or the AppData/Roaming/program_name directory.
        std::filesystem::path getWinBestDataDirPath() const;
        // Appends the schedule save subdirectory to the best available save dir path.
        std::filesystem::path getBestScheduleSavePath() const;
        // Appends the configs save subdirectory to the best available data dir path.
        std::filesystem::path getBestConfigSavePath() const;

    public:
        // Initialise the IO handler and the specific IO classes.
        void init(Schedule* schedule, Window* window, Input& input, Interface& interface);
        void addToAutosaveTimer(double delta);
        std::shared_ptr<ScheduleIO> getScheduleIO();
        std::shared_ptr<PreferencesIO> getPreferencesIO();
};
