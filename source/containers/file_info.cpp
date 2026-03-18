#include "file_info.h"

FileInfo::FileInfo(const std::filesystem::path& path,
                   const TimeWrapper& fileEditTime,
                   const std::optional<TimeWrapper>& scheduleEditTime) {
    m_path = path;
    m_fileEditTime = fileEditTime;
    m_scheduleEditTime = scheduleEditTime;
}

std::filesystem::path FileInfo::getPath() const {
    return m_path;
}

std::string FileInfo::getStem() const {
    return m_path.stem().string();
}

std::string FileInfo::getFilename() const {
    return m_path.filename().string();
}

void FileInfo::rename(const std::string& name) {
    if (name.empty()) {
        return;
    }

    std::string extension = m_path.extension().string();

    m_path.replace_filename(name);

    // Allow file extension to be *changed*, but not *removed*
    if (!m_path.has_extension()) {
        m_path.replace_extension(extension);
    }
}

FileInfo FileInfo::getRenamed(const std::string& name) {
    auto newInfo = FileInfo(*this);

    newInfo.rename(name);
    return newInfo;
}

TimeWrapper FileInfo::getFileEditTime() const {
    return m_fileEditTime;
}

void FileInfo::setFileEditTime(const TimeWrapper& editTime) {
    m_fileEditTime = editTime;
}

std::optional<TimeWrapper> FileInfo::getScheduleEditTime() const {
    return m_scheduleEditTime;
}

void FileInfo::setScheduleEditTime(const TimeWrapper& editTime) {
    m_scheduleEditTime = editTime;
}