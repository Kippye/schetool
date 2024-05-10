cmake_minimum_required(VERSION 3.25)

project(schetool VERSION 0.5)

# FIRST just get it to build on Windows using Makefiles, worry about all the filters and Linux later!
# specify the C++ standard
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

set(CMAKE_CXX_STANDARD 17)

# TEMP?
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++")

# BLF (has its own CMakeLists!)
# glad (has nothing lmao, but i can probably add my own CMakeLists there since it's not a submodule?)
# glfw (has a CMakeLists)
# glm (has a CMakeLists)
# imgui (purely headers and cpp files)
# zlib (has a CMakeLists) (should be BLF's responsibility :/ ) 

add_subdirectory("include/blf")
add_subdirectory("include/glad")
add_subdirectory("include/glfw")
add_subdirectory("include/glm")
add_subdirectory("include/imgui") # TODO
# TEMP?

add_subdirectory("include/zlib")

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
    "${SCHETOOL_SRC}/imgui_demo.cpp"
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

target_link_libraries(schetool PUBLIC compiler_flags blf glad glfw glm imgui zlib)