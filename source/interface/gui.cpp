#include <gui.h>

GuiDrawArgs::GuiDrawArgs(
    const WindowSize& windowSize, Input& input, GuiTextures& guiTextures, Preferences preferences, float deltaTime)
    : windowSize(windowSize), input(input), guiTextures(guiTextures), preferences(preferences), deltaTime(deltaTime) {
}

Gui::Gui() {
}

Gui::Gui(const char* ID) {
    m_ID = std::string(ID);
}

std::string Gui::getID() const {
    return m_ID;
}

bool Gui::getVisible() const {
    return m_visible;
}

// NOTE: Guis should not modify m_visible themselves. So this is the main way to make them visible / hidden
void Gui::setVisible(bool visible) {
    m_visible = visible;
}

void Gui::draw(GuiDrawArgs& args) {
}

std::string Gui::addSubGui(Gui* subGui) {
    std::shared_ptr<Gui> subGuiPtr = std::shared_ptr<Gui>(subGui);
    if (subGuiPtr) {
        subGuis.insert({subGuiPtr->getID(), subGuiPtr});
        return subGuiPtr->getID();
    } else {
        printf("Gui::addSubGui(%s, %p): Failed to add subGui - invalid pointer\n", "ID", (void*)subGui);
        return "";
    }
}