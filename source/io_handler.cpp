#include "data_converter.h"
#include "schedule.h"
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

void IO_Handler::writeSchedule(const char* name)
{
    std::string relativePath = makeRelativePathFromName(name);

    if (m_converter.writeSchedule(relativePath.c_str(), m_schedule->getColumns()) == 0)
    {
        std::cout << "Successfully wrote Schedule to file: " << relativePath << std::endl;
    }
    m_schedule->setEditedSinceWrite(false);
}

// Reads a schedule from file and applies / opens it
void IO_Handler::readSchedule(const char* name)
{
    std::string relativePath = makeRelativePathFromName(name);
    std::vector<Column> columnsCopy = m_schedule->getColumns();

    if (m_converter.readSchedule(relativePath.c_str(), columnsCopy) == 0)
    {
        m_schedule->replaceSchedule(columnsCopy);
        std::cout << "Successfully read Schedule from file: " << relativePath << std::endl;
    }
    m_schedule->setEditedSinceWrite(false);
}

const char* IO_Handler::getOpenScheduleFilename()
{
    return m_openScheduleFilename;
}

std::vector<std::string> IO_Handler::getScheduleStemNames()
{
    std::vector<std::string> filenames = {};
    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);

    for (const auto& entry: fs::directory_iterator(schedulesPath))
    {
        filenames.push_back(entry.path().stem().string());
    }
    return filenames;
}

std::string IO_Handler::getLastEditedScheduleStemName()
{
    long long latestEditTime = std::numeric_limits<long long>::min();
    fs::path latestEditedPath;
    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    fs::directory_iterator dirIterator = fs::directory_iterator(schedulesPath);

    for (const auto& entry: dirIterator)
    {
        fs::file_time_type editTime = fs::last_write_time(entry);

        else if (latestEditTime == std::numeric_limits<long long>::min() || editTime.time_since_epoch().count() > latestEditTime)
        {
            latestEditedPath = entry;
            latestEditTime = editTime.time_since_epoch().count();
        }
    }

    return latestEditedPath.stem().string();
}