#pragma once

#include <utility>

class ScheduleCoordinates {
    private:
        size_t m_column;
        size_t m_row;

    public:
        ScheduleCoordinates(size_t column, size_t row) : m_column(column), m_row(row) {
        }

        bool operator==(const ScheduleCoordinates& other) const {
            return m_column == other.m_column && m_row == other.m_row;
        }

        bool operator!=(const ScheduleCoordinates& other) const {
            return !(m_column == other.m_column && m_row == other.m_row);
        }

        bool is(size_t column, size_t row) const {
            return m_column == column && m_row == row;
        }

        std::pair<size_t, size_t> getAsPair() const {
            return std::pair(m_column, m_row);
        }

        size_t column() const {
            return m_column;
        }

        size_t row() const {
            return m_row;
        }
};