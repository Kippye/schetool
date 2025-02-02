#include "io_handler.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <shlobj.h>
#endif

namespace fs = std::filesystem;

fs::path IO_Handler::getWinBestDataDirPath() const
{
    #ifdef SCHETOOL_WINDOWS
    std::filesystem::path path;
    PWSTR path_tmp;

    // Attempt to get user's AppData folder
    HRESULT get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);

    // Error check
    if (get_folder_path_ret != S_OK) {
        CoTaskMemFree(path_tmp);
        return ".";
    }

    // Convert the Windows path type to a C++ path
    path = path_tmp;

    // Free memory :)
    CoTaskMemFree(path_tmp);
    path /= "schetool";
    return path;
    #else
    return ".";
    #endif
}

fs::path IO_Handler::getBestScheduleSavePath() const
{
    // The existence of a relative schedules dir overrides other save paths
    if (fs::exists("./schedules"))
    {
        return "./schedules";
    }
    #if defined (SCHETOOL_WINDOWS)
    return getWinBestDataDirPath() / "schedules";
    #elif defined(SCHETOOL_LINUX)
    fs::path base = std::getenv("HOME") ? fs::path(std::getenv("HOME")) / ".local/share" : ".";
    // If the home/.local/share directory doesn't exist, it would be pretty weird for schetool to create it.
    base = fs::exists(base) ? base : fs::path(".");
    return base / "schetool/schedules";
    #endif
}

fs::path IO_Handler::getBestConfigSavePath() const
{
    // The existence of a relative config dir overrides other save paths
    if (fs::exists("./config"))
    {
        return "./config";
    }
    #if defined(SCHETOOL_WINDOWS)
    return getWinBestDataDirPath() / "config";
    #elif defined(SCHETOOL_LINUX)
    fs::path base = std::getenv("HOME") ? fs::path(std::getenv("HOME")) / ".config" : ".";
    // If the home/.config directory doesn't exist, it would be pretty weird for schetool to create it.
    base = fs::exists(base) ? base : fs::path(".");
    return base / "schetool/config";
    #endif
}

void IO_Handler::init(Schedule* schedule, Window* window, Input& input, Interface& programInterface)
{
    m_schedule = schedule;
    m_windowManager = window;

    // EVENT LISTENERS
    m_mainMenuBarGui = programInterface.getGuiByID<MainMenuBarGui>("MainMenuBarGui");
    if (m_mainMenuBarGui)
    {
        m_mainMenuBarGui->preferencesChangedEvent.addListener(preferencesModifiedListener);
    }

    window->windowCloseEvent.addListener(windowCloseListener);
    input.addEventListener(INPUT_EVENT_SC_SAVE, saveInputListener);

    fs::path savesDir = getBestScheduleSavePath();
    fs::path configDir = getBestConfigSavePath();
    std::cout << std::format("Schedules save path is: '{}'", savesDir.string()) << std::endl;
    std::cout << std::format("Config save path is: '{}'", configDir.string()) << std::endl;

    // TODO: I don't like this pointer dereferencing
    m_scheduleIO = std::make_shared<ScheduleIO>(*schedule, programInterface, savesDir);
    m_preferencesIO = std::make_shared<PreferencesIO>(configDir);
    
    m_scheduleIO->openFileInfoChangeEvent.addListener(openFileInfoChangeListener);
    m_preferencesIO->preferencesLoadedEvent.addListener(preferencesLoadedListener);
}

void IO_Handler::addToAutosaveTimer(double delta)
{
    m_timeSinceAutosave += delta;

    if (m_timeSinceAutosave > (double)AUTOSAVE_DELAY_SECONDS)
    {
        if (m_scheduleIO)
        {
            m_scheduleIO->createAutosave();
        }
        m_timeSinceAutosave = 0;
    }
}

std::shared_ptr<ScheduleIO> IO_Handler::getScheduleIO()
{
    if (!m_scheduleIO)
    {
        std::cout << "WARNING: IO_Handler::getScheduleIO(): Returned shared_ptr is empty." << std::endl;
    }
    return m_scheduleIO;
}

std::shared_ptr<PreferencesIO> IO_Handler::getPreferencesIO()
{
    if (!m_preferencesIO)
    {
        std::cout << "WARNING: IO_Handler::getPreferencesIO(): Returned shared_ptr is empty." << std::endl;
    }
    return m_preferencesIO;
}