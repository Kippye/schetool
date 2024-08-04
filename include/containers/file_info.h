#pragma once

#include <string>
#include "time_wrapper.h"

class FileInfo
{
    private:
        bool m_isEmpty = true;
        std::string m_filename = "";
        TimeWrapper m_fileEditTime;
        TimeWrapper m_scheduleEditTime;
    public:
        FileInfo();
        FileInfo(const std::string& filename, const TimeWrapper& fileEditTime, const TimeWrapper& scheduleEditTime);

        bool empty() const;
        std::string getName() const;
        // Change the contained name. 
        // NOTE: Only use when the file itself was renamed! To change the contained file, use fill().
        void rename(const std::string& name);
        TimeWrapper getFileEditTime() const;
        // Changes file edit time if the FileInfo is full
        void setFileEditTime(const TimeWrapper& editTime);
        TimeWrapper getScheduleEditTime() const;
        // Changes schedule edit time if the FileInfo is full
        void setScheduleEditTime(const TimeWrapper& editTime);

        void clear();
        void fill(const std::string& filename, const TimeWrapper& fileEditTime, const TimeWrapper& scheduleEditTime);
};