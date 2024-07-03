#include <gui.h>

Gui::Gui()
{
    
}

Gui::Gui(const char* ID)
{
    m_ID = std::string(ID);
}

std::string Gui::getID() const
{
    return m_ID;
}

bool Gui::getVisible() const
{
    return m_visible;
}

// NOTE: Guis should not modify m_visible themselves. So this is the main way to make them visible / hidden
void Gui::setVisible(bool visible)
{
    m_visible = true;
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