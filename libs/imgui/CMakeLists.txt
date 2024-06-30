set(IMGUI_SRC "./src")
add_library(imgui
"${IMGUI_SRC}/imgui.cpp"
"${IMGUI_SRC}/imgui_draw.cpp"
"${IMGUI_SRC}/imgui_impl_glfw.cpp"
"${IMGUI_SRC}/imgui_impl_opengl3.cpp"
"${IMGUI_SRC}/imgui_stdlib.cpp"
"${IMGUI_SRC}/imgui_tables.cpp"
"${IMGUI_SRC}/imgui_widgets.cpp"
)
target_include_directories(imgui PUBLIC "./include")