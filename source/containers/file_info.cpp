#include "file_info.h"

FileInfo::FileInfo() : m_isEmpty(true)
{}

FileInfo::FileInfo(const std::string& filename, const TimeWrapper& fileEditTime, const TimeWrapper& scheduleEditTime)
{
    fill(filename, fileEditTime, scheduleEditTime);
}

bool FileInfo::empty() const
{
    return m_isEmpty;
}

std::string FileInfo::getName() const 
{
    return m_filename;
}

void FileInfo::rename(const std::string& name)
{
    if (empty() || name.empty()) { return; }

    m_filename = name;
}

TimeWrapper FileInfo::getFileEditTime() const
{
    return m_fileEditTime;
}

void FileInfo::setFileEditTime(const TimeWrapper& editTime)
{
    if (empty()) { return; }

    m_fileEditTime = editTime;
}

TimeWrapper FileInfo::getScheduleEditTime() const
{
    return m_scheduleEditTime;
}

void FileInfo::setScheduleEditTime(const TimeWrapper& editTime)
{
    if (empty()) { return; }

    m_scheduleEditTime = editTime;
}

void FileInfo::clear()
{
    m_filename = "";
    m_fileEditTime = TimeWrapper();
    m_scheduleEditTime = TimeWrapper();
    m_isEmpty = true;
}

void FileInfo::fill(const std::string& filename, const TimeWrapper& fileEditTime, const TimeWrapper& scheduleEditTime)
{
    m_filename = filename;
    m_fileEditTime = fileEditTime;
    m_scheduleEditTime = scheduleEditTime;
    m_isEmpty = false;
}