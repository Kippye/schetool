#pragma once

class WindowSize {
    private:
        size_t m_screenWidth;
        size_t m_screenHeight;

    public:
        WindowSize() : m_screenWidth(0), m_screenHeight(0) {
        }
        WindowSize(size_t width, size_t height) : m_screenWidth(width), m_screenHeight(height) {
        }
        size_t getWidth() const {
            return m_screenWidth;
        }
        size_t getHeight() const {
            return m_screenHeight;
        }
};