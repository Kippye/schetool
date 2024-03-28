#include "data_converter.h"
#include "schedule.h"
#include <io_handler.h>

void IO_Handler::init(Schedule* schedule)
{
    m_schedule = schedule;
    m_converter = DataConverter();
    m_converter.setupObjectTable();
}

void IO_Handler::writeSchedule(const char* path)
{
    if (m_converter.writeSchedule(path, m_schedule->getColumns()) == 0)
    {
        std::cout << "Successfully wrote Schedule to file: " << path << std::endl;
    }
    m_schedule->setEditedSinceWrite(false);
}

// Reads a schedule from file and applies / opens it
void IO_Handler::readSchedule(const char* path)
{
    std::vector<Column> columnsCopy = m_schedule->getColumns();

    if (m_converter.readSchedule(path, columnsCopy) == 0)
    {
        m_schedule->replaceSchedule(columnsCopy);
        std::cout << "Successfully read Schedule from file: " << path << std::endl;
    }
    m_schedule->setEditedSinceWrite(false);
}