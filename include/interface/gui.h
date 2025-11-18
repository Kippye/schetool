#pragma once
#include <memory>
#include <string>
#include <type_traits>
#include <map>
#include "imgui/include/imgui.h"
// These are only here to reduce required includes in gui classes
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_glfw.h"
#include "imgui/include/imgui_impl_opengl3.h"
#include "event.h"
// Draw args
#include "window_size.h"
#include "input.h"
#include "gui_textures.h"
#include "preferences.h"

struct GuiDrawArgs {
        WindowSize windowSize;
        Input& input;
        GuiTextures& guiTextures;
        Preferences preferences;

        GuiDrawArgs(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures, Preferences preferences);
};

class Gui {
    protected:
        std::string m_ID;
        bool m_visible = true;

    public:
        Gui();
        Gui(const char* ID);
        virtual ~Gui() = default;

        ImVec2 position;
        ImVec2 size;
        float opacity = 1.0f;
        std::map<std::string, std::shared_ptr<Gui>> subGuis = {};

        std::string getID() const;
        bool getVisible() const;
        virtual void setVisible(bool visible);
        virtual void draw(GuiDrawArgs& args);
        // Add a subgui and return its ID so it can be easily retrieved with getSubGui
        std::string addSubGui(Gui* subGui);
        template <typename T>
        std::shared_ptr<T> getSubGui(const std::string& ID) {
            static_assert(std::is_base_of_v<Gui, T>, "Gui::getSubGui<T>: Provided type must derive from Gui!");
            if (subGuis.find(ID) == subGuis.end()) {
                printf("Gui::getSubGui(ID: %s): Failed to get subGui of Gui with ID: %s. Returning shared_ptr to nullptr\n",
                       ID.c_str(),
                       m_ID.c_str());
                return std::shared_ptr<T>(nullptr);
            }
            return std::dynamic_pointer_cast<T>(subGuis.at(ID));
        }
};