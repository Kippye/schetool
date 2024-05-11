cmake_minimum_required(VERSION 3.25)

project(schetool VERSION 0.5)
# TODO target_compile_definitions() for version number

# specify the C++ standard
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

# TODO: Use something to check if these libraries exist on the system
# INCLUDE (CheckLibraryExists)
# Usage: CHECK_LIBRARY_EXISTS(library function location variable)

message("Setting compile flags for ${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    if ((CMAKE_SYSTEM_NAME STREQUAL Windows) OR (CMAKE_SYSTEM_NAME STREQUAL MSYS)) 
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -static -mtune=native -lpthread -static-libstdc++ -static-libgcc -std=c++17 -lstdc++")
        message("Set Unix Makefiles compile flags for Windows or MSYS system to: ${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++")
        message("Set Unix Makefiles base compile flags to: ${CMAKE_CXX_FLAGS}")
    endif()
endif()
message("Finished setting compile flags")

# package libraries
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# local source-compiled libraries
add_subdirectory("include/blf")
add_subdirectory("include/glad")

add_subdirectory("include/imgui")
target_link_libraries(imgui glfw) # imgui is dependent on glfw and causes a compile error if not linked with it here

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