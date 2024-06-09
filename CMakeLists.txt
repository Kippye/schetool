cmake_minimum_required(VERSION 3.25)

project(schetool VERSION 0.5.0)

set(PROGRAM_NAME ${PROJECT_NAME})

# enable testing by default for debug makefile builds (can't check for configuration with msvc)
IF ((CMAKE_GENERATOR STREQUAL "Unix Makefiles") AND ("${CMAKE_BUILD_TYPE}" STREQUAL Debug))
    option(ENABLE_TESTING "Unit testing build" ON)
# otherwise, testing has to be manually enabled
ELSE()
    option(ENABLE_TESTING "Unit testing build" OFF)
ENDIF()

# specify the C++ standard
add_library(compiler_flags INTERFACE)

# add compile options defined by preset (might be empty)
target_compile_options(compiler_flags INTERFACE ${PRESET_ADD_COMPILE_OPTIONS})

# TODO: Use something to check if these libraries exist on the system
# INCLUDE (CheckLibraryExists)
# Usage: CHECK_LIBRARY_EXISTS(library function location variable)

message(STATUS "~~ Setting compile and link flags for ${CMAKE_GENERATOR}")
IF (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    IF ((CMAKE_SYSTEM_NAME STREQUAL Windows) OR (CMAKE_SYSTEM_NAME STREQUAL MSYS)) 
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -static -mtune=native -lpthread -static-libstdc++ -static-libgcc -std=c++17 -lstdc++")
        message(STATUS "~~ Set Unix Makefiles compile flags for Windows or MSYS system to: ${CMAKE_CXX_FLAGS}")
        # keep console for debug builds and hide it for release builds
        # /ENTRY:mainCRTStartup keeps the same "main" function instead of requiring "WinMain"
        IF (${CMAKE_BUILD_TYPE} STREQUAL Debug)
            set(SUBSYSTEM_LINKER_OPTIONS "-Wl,-subsystem,console" "-Wl,-entry,mainCRTStartup")
            message(STATUS "~~ Added Windows Debug linker options: ${SUBSYSTEM_LINKER_OPTIONS}")
            set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
            message(DEBUG "~~ Enabled exporting compile commands")
        ELSEIF(${CMAKE_BUILD_TYPE} STREQUAL Release)
            set(SUBSYSTEM_LINKER_OPTIONS "-Wl,-subsystem,windows" "-Wl,-entry,mainCRTStartup")
            message(STATUS "~~ Added Windows Release linker options: ${SUBSYSTEM_LINKER_OPTIONS}")
        ENDIF()
    ELSE()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++")
        message(STATUS "~~ Set Unix Makefiles base compile flags to: ${CMAKE_CXX_FLAGS}")
    ENDIF()
ELSEIF (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    message(STATUS "~~ Set Visual Studio compile flags to: ${CMAKE_CXX_FLAGS}")
ENDIF()
message(STATUS "~~ Finished setting compile flags")

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# local source-compiled libraries
add_subdirectory("include/blf")
add_subdirectory("include/glad")

add_subdirectory("include/imgui")
target_link_libraries(imgui glfw) # imgui is dependent on glfw and causes a compile error if not linked with it here


# pass information like PROGRAM_NAME, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR and PROJECT_VERSION_PATCH to a header file to include
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/program_info.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/generated/program_info.h")

# TODO: option(TEST "Should tests be performed?" ON)

# executable
IF (ENABLE_TESTING)
    add_library(${PROGRAM_NAME})
ELSE()
    add_executable(${PROGRAM_NAME})
ENDIF()

# add platform-specific defines and options
IF (WIN32)
    target_compile_definitions(${PROGRAM_NAME} PUBLIC "WIN32")
    target_compile_definitions(${PROGRAM_NAME} PUBLIC "_WIN32")
    message(STATUS "~~ Added Windows compile definitions")
ELSE()
    
ENDIF()

target_link_options(${PROGRAM_NAME} PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})

# add build configuration defines
target_compile_definitions(${PROGRAM_NAME} PUBLIC "$<$<CONFIG:Debug>:DEBUG>")
target_compile_definitions(${PROGRAM_NAME} PUBLIC "$<$<NOT:$<CONFIG:Debug>>:NDEBUG>")

IF (ENABLE_TESTING)
    target_compile_definitions(${PROGRAM_NAME} PUBLIC "PERFORM_UNIT_TESTS")
ENDIF (ENABLE_TESTING)

target_include_directories(${PROGRAM_NAME} PUBLIC "include" PUBLIC "include/generated")

set(SCHETOOL_SRC "source")
target_sources(${PROGRAM_NAME} PRIVATE
    "${SCHETOOL_SRC}/data_converter.cpp"
    "${SCHETOOL_SRC}/date_container.cpp"
    "${SCHETOOL_SRC}/edit_history_gui.cpp"
    "${SCHETOOL_SRC}/element_base.cpp"
    "${SCHETOOL_SRC}/filter_base.cpp"
    "${SCHETOOL_SRC}/gui.cpp"
    #"${SCHETOOL_SRC}/imgui_demo.cpp"
    "${SCHETOOL_SRC}/input.cpp"
    "${SCHETOOL_SRC}/interface.cpp"
    "${SCHETOOL_SRC}/io_handler.cpp"
    "${SCHETOOL_SRC}/main_menu_bar_gui.cpp"
    "${SCHETOOL_SRC}/main.cpp"
    "${SCHETOOL_SRC}/render.cpp"
    "${SCHETOOL_SRC}/schedule_column.cpp"
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

target_link_libraries(${PROGRAM_NAME} PUBLIC compiler_flags 
    blf
    imgui
    glad 
    glfw
    glm::glm
)

# add testing subdirectory (handles its own logic)
add_subdirectory(tests)