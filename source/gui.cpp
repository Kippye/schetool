#include <gui.h>

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

void Gui::addSubGui(const std::string& ID, Gui* subGui)
{
    std::shared_ptr<Gui> subGuiPtr = std::shared_ptr<Gui>(subGui);
    if (subGuiPtr)
    {
        subGuis.insert({ID, subGuiPtr});
    }
    else
    {
        printf("Gui::addSubGui(%s, %p): Failed to add subGui - invalid pointer\n", ID.c_str(), (void*)subGui);
    }
}