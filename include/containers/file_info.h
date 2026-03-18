#pragma once

#include "time_wrapper.h"
#include <string>
#include <filesystem>
#include <optional>

class FileInfo {
    private:
        std::filesystem::path m_path;
        TimeWrapper m_fileEditTime;
        std::optional<TimeWrapper> m_scheduleEditTime = std::nullopt;

    public:
        FileInfo() = delete;
        // Full physical file description
        FileInfo(const std::filesystem::path& path,
                 const TimeWrapper& fileEditTime = TimeWrapper::getCurrentTime(),
                 const std::optional<TimeWrapper>& scheduleEditTime = std::nullopt);

        bool operator==(const FileInfo& other) {
            return m_path == other.m_path;
        }

        bool operator!=(const FileInfo& other) {
            return m_path != other.m_path;
        }

        // Get the full file path
        std::filesystem::path getPath() const;
        // Get the filename stem (no extension)
        std::string getStem() const;
        // Get the filename (with extension)
        std::string getFilename() const;
        // Change the filename of the contained file's path.
        // Provide either only the stem or the full filename.
        // NOTE: Only use when the file itself was renamed! To change the contained file, use fill().
        void rename(const std::string& name);
        // Get a new FileInfo with the the filename of the contained file's path renamed.
        // Provide either only the stem or the full filename.
        FileInfo getRenamed(const std::string& name);
        TimeWrapper getFileEditTime() const;
        // Changes file edit time if the FileInfo is full
        void setFileEditTime(const TimeWrapper& editTime);
        std::optional<TimeWrapper> getScheduleEditTime() const;
        // Changes schedule edit time if the FileInfo is full
        void setScheduleEditTime(const TimeWrapper& editTime);
};