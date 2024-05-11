cmake_minimum_required(VERSION 3.25)

project(schetool VERSION 0.5)

# FIRST just get it to build on Windows using Makefiles, worry about all the filters and Linux later!
# specify the C++ standard
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

set(CMAKE_CXX_STANDARD 17)

# TEMP?
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++")

# packages
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
#find_package(imgui REQUIRED)
#     {
#     "name": "imgui",
#     "features": [
#       "glfw-binding",
#       "opengl3-binding"
#     ]
#   },

# include(FetchContent)

# FetchContent_Declare(
#   Imgui
#   GIT_REPOSITORY https://github.com/ocornut/imgui.git
#   GIT_TAG        277ae93c41314ba5f4c7444f37c4319cdf07e8cf # 1.90.4
#   FIND_PACKAGE_ARGS NAMES imgui
# )

# FetchContent_MakeAvailable(Imgui)

# find_package(imgui
#     REQUIRED
#     NO_DEFAULT_PATH
#     PATHS ${imgui_SOURCE_DIR}
# )

add_subdirectory("include/blf")
add_subdirectory("include/glad")
# add_subdirectory("include/glfw")
#add_subdirectory("include/glm")
add_subdirectory("include/imgui") # TODO
# TEMP?

target_link_libraries(imgui glfw)

# executable
add_executable(schetool)

target_include_directories(schetool PRIVATE "include")
set(SCHETOOL_SRC "source")
target_sources(schetool PRIVATE
    "${SCHETOOL_SRC}/data_converter.cpp"
    "${SCHETOOL_SRC}/date_container.cpp"
    "${SCHETOOL_SRC}/edit_history_gui.cpp"
    "${SCHETOOL_SRC}/element_base.cpp"
    "${SCHETOOL_SRC}/gui.cpp"
    #"${SCHETOOL_SRC}/imgui_demo.cpp"
    "${SCHETOOL_SRC}/input.cpp"
    "${SCHETOOL_SRC}/interface.cpp"
    "${SCHETOOL_SRC}/io_handler.cpp"
    "${SCHETOOL_SRC}/main_menu_bar_gui.cpp"
    "${SCHETOOL_SRC}/main.cpp"
    "${SCHETOOL_SRC}/render.cpp"
    "${SCHETOOL_SRC}/schedule_core.cpp"
    "${SCHETOOL_SRC}/schedule_edit_history.cpp"
    "${SCHETOOL_SRC}/schedule_edit.cpp"
    "${SCHETOOL_SRC}/schedule_gui.cpp"
    "${SCHETOOL_SRC}/schedule.cpp"
    "${SCHETOOL_SRC}/select_container.cpp"
    "${SCHETOOL_SRC}/select_options.cpp"
    "${SCHETOOL_SRC}/textures.cpp"
    "${SCHETOOL_SRC}/time_container.cpp"
    "${SCHETOOL_SRC}/window.cpp"
)

target_link_libraries(schetool PUBLIC compiler_flags 
    blf
    imgui
    glad 
    glfw
    glm::glm
)