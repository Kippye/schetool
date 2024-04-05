#include <gui.h>
#include <window.h>

Gui::Gui()
{
    
}

Gui::Gui(const char* ID)
{
    m_ID = std::string(ID);
}

std::string Gui::getID()
{
    return m_ID;
}

void Gui::draw(Window& window, Input& input)
{
}