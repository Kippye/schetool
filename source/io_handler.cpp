#include "data_converter.h"
#include "schedule.h"
#include <algorithm>
#include <chrono>
#include <io_handler.h>
#include <filesystem>
#include <limits>

namespace fs = std::filesystem;

std::string IO_Handler::makeRelativePathFromName(const char* name)
{
    return std::string(SCHEDULES_SUBDIR_PATH).append(std::string(name)).append(std::string(SCHEDULE_FILE_EXTENSION));
}

void IO_Handler::init(Schedule* schedule)
{
    m_schedule = schedule;
    m_converter = DataConverter();
    m_converter.setupObjectTable();
}

bool IO_Handler::writeSchedule(const char* name)
{
    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    if (fs::exists(schedulesPath) == false)
    {
        std::cout << "Tried to write Schedule but the schedules directory did not exist. Created a schedules directory." << std::endl;
        fs::create_directory(schedulesPath);
    }

    std::string relativePath = makeRelativePathFromName(name);

    if (m_converter.writeSchedule(relativePath.c_str(), m_schedule->getColumns()) == 0)
    {
        std::cout << "Successfully wrote Schedule to file: " << relativePath << std::endl;
    }
    m_schedule->setEditedSinceWrite(false);
    return true;
}

// Reads a schedule from file and applies / opens it
bool IO_Handler::readSchedule(const char* name)
{    
    std::string relativePath = makeRelativePathFromName(name);
    
    if (fs::exists(fs::path(relativePath.c_str())) == false)
    {
        std::cout << "Tried to read Schedule at path to non-existant file:" << relativePath << std::endl;
        return false;
    }

    std::vector<Column> columnsCopy = m_schedule->getColumns();

    if (m_converter.readSchedule(relativePath.c_str(), columnsCopy) == 0)
    {
        m_schedule->replaceSchedule(columnsCopy);
        std::cout << "Successfully read Schedule from file: " << relativePath << std::endl;
    }
    setOpenScheduleFilename(std::string(name));
    m_schedule->setEditedSinceWrite(false);
    return true;
}

bool IO_Handler::createNewSchedule(const char* name)
{
    m_schedule->createDefaultSchedule();

    if (writeSchedule(name))
    {
        setOpenScheduleFilename(std::string(name));
        return true;
    }
    return false;
}

void IO_Handler::addToAutosaveTimer(double delta)
{
    m_timeSinceAutosave += delta;

    if (m_timeSinceAutosave > (double)AUTOSAVE_DELAY_SECONDS)
    {
        if (m_schedule->getEditedSinceWrite() == true)
        {
            writeSchedule(m_openScheduleFilename.c_str());
        }
        m_timeSinceAutosave = 0;
    }
}

std::string IO_Handler::getOpenScheduleFilename()
{
    return m_openScheduleFilename;
}

bool IO_Handler::setOpenScheduleFilename(const std::string& name, bool renameFile)
{
    if (renameFile)
    {
        fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
        fs::path pathToOpenFile = fs::path(makeRelativePathFromName(m_openScheduleFilename.c_str()));
        fs::path pathToRenamedFile = fs::path(makeRelativePathFromName(name.c_str()));
        bool schedulesDirWasCreated = false;

        // Write-operation: Create schedules directory if it doesn't exist.
        if (fs::exists(schedulesPath) == false)
        {
            std::cout << "Tried to rename a Schedule but the schedules directory did not exist. Created a schedules directory." << std::endl;
            fs::create_directory(schedulesPath);
            schedulesDirWasCreated = true;
        }
        //  A Schedule file with this name already exists, don't overwrite it. Just stop.
        if (fs::exists(pathToRenamedFile))
        {
            return false;
        }
        // If the file to rename doesn't exist, just write the Schedule to the file with the provided new name
        if (schedulesDirWasCreated || fs::exists(pathToOpenFile) == false)
        {
            std::cout << "Tried to change the name of the Schedule file, but the file was not found at its previous path:" << pathToOpenFile.string() << std::endl;
            writeSchedule(name.c_str());
            std::cout << "Wrote current file to renamed path:" << pathToRenamedFile.string() << std::endl;
        }
        else // All is fine, rename the file
        {
            fs::rename(pathToOpenFile, pathToRenamedFile);
        }
    }

    m_openScheduleFilename = name;
    m_schedule->setScheduleName(name);
    return true;
}

std::vector<std::string> IO_Handler::getScheduleStemNames()
{
    std::vector<std::string> filenames = {};
    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    if (fs::exists(schedulesPath) == false)
    {
        std::cout << "Tried to get Schedule stem names but the schedules directory did not exist." << std::endl;
        return filenames;
    }

    for (const auto& entry: fs::directory_iterator(schedulesPath))
    {
        filenames.push_back(entry.path().stem().string());
    }
    return filenames;
}

std::vector<std::string> IO_Handler::getScheduleStemNamesSortedByEditTime()
{
    std::vector<std::string> filenames = {};
    std::map<std::string, long long> filenamesEditTimes = {};

    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    if (fs::exists(schedulesPath) == false)
    {
        std::cout << "Tried to get sorted Schedule stem names but the schedules directory did not exist." << std::endl;
        return filenames;
    }

    for (const auto& entry: fs::directory_iterator(schedulesPath))
    {
        filenames.push_back(entry.path().stem().string());
        filenamesEditTimes.insert({entry.path().stem().string(), fs::last_write_time(entry).time_since_epoch().count()}); 
    }

    std::sort(filenames.begin(), filenames.end(), [&](std::string fs1, std::string fs2){ return filenamesEditTimes.at(fs1) > filenamesEditTimes.at(fs2); });
    return filenames;
}

std::string IO_Handler::getLastEditedScheduleStemName()
{
    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    if (fs::exists(schedulesPath) == false)
    {
        std::cout << "Tried to get the latest edited Schedule, but the schedules directory did not exist." << std::endl;
        return std::string("");
    }

    long long latestEditTime = std::numeric_limits<long long>::min();
    fs::path latestEditedPath;
    fs::directory_iterator dirIterator = fs::directory_iterator(schedulesPath);

    for (const auto& entry: dirIterator)
    {
        fs::file_time_type editTime = fs::last_write_time(entry);

        if (latestEditTime == std::numeric_limits<long long>::min() || editTime.time_since_epoch().count() > latestEditTime)
        {
            latestEditedPath = entry;
            latestEditTime = editTime.time_since_epoch().count();
        }
    }

    return latestEditedPath.stem().string();
}