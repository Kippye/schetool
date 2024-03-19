#include <algorithm>
#include <cctype>
#include <ctime>
#include <iterator>
#include <string.h>
#include <weekday_container.h>
#include <iostream>

Weekday::Weekday()
{

}

// Returns the string representation of the stored time, depending on the displayDate, displayTime and displayWeekday values.
std::string Weekday::getString()
{
    try
    {
        char output[1024];

        unsigned int currentIndex = 0;
        tm weekdayDisplay;

        // display all selected weekdays
        for (int i = 0; i < sizeof(m_selectedWeekdays); i++)
        {
            if (m_selectedWeekdays[i] == true)
            {
                weekdayDisplay.tm_wday = i == 0 ? 1 : (i == 6 ? 0 : i + 1);
                char currentOutput[32];
                std::strftime(currentOutput, sizeof(currentOutput), "%A ", &weekdayDisplay);
                // copy to total output
                for (int j = 0; j < sizeof(currentOutput); j++)
                {
                    if (std::isalpha((unsigned char)currentOutput[j]) != 0 || std::isblank((unsigned char)currentOutput[j]))
                    {
                        output[currentIndex] = currentOutput[j];
                        currentIndex++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        return std::string(output);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        std::cout << "std::string Weekday::getString(): Failed to convert current weekdays to a string." << std::endl;
        return std::string("");
    }
}  

const bool* Weekday::getSelectedWeekdays()
{
    return m_selectedWeekdays;
}

// Set the weekday at index weekday (0-6) selection to selected
void Weekday::setWeekdaySelected(unsigned int weekday, bool selected)
{
    weekday = std::min(weekday, 6U);
    m_selectedWeekdays[weekday] = selected;
}