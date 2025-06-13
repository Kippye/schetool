cmake_minimum_required(VERSION 3.25)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

############ PROJECT DEFINITION ############
project(schetool VERSION 0.9.0)
set(CPP_VERSION "c++20")

######### OPTIONS #########
# Enable testing by default for Debug Unix Makefiles (can't check for configuration with MSVC)
IF ((CMAKE_GENERATOR STREQUAL "Unix Makefiles") AND ("${CMAKE_BUILD_TYPE}" STREQUAL Debug))
    option(ENABLE_TESTS "Unit testing build" ON)
# Otherwise, testing has to be manually enabled
ELSE()
    option(ENABLE_TESTS "Unit testing build" OFF)
ENDIF()

# Force even Release builds to show and output to a console
option(FORCE_CONSOLE "Should the console be visible even in Release builds?" OFF)

######### GLOBALS #########
set(PROGRAM_NAME ${PROJECT_NAME})
### SUBDIRS ###
set(LIBRARY_DIR "libs")
set(SCHETOOL_SRC    "source")
### SUB-SUBDIRS ###
set(CONSTANTS_DIR   "constants")
set(CONTAINERS_DIR  "containers")
set(HANDLERS_DIR    "handlers")
set(INTERFACE_DIR   "interface")
set(IO_DIR          "io")
set(SCHEDULE_DIR    "schedule")
set(UTIL_DIR        "util")
### INSTALL ###
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
### PACKAGING ###
set(PACKAGING_DIR "${CMAKE_SOURCE_DIR}/packaging")

######### MAIN TARGET #########
IF (ENABLE_TESTS)
    add_library(${PROGRAM_NAME})
ELSE()
    add_executable(${PROGRAM_NAME})
ENDIF()

######### PLATFORM #########
include("cmake/platform.cmake")

######### PACKAGES #########
message("~~~~ Configuring ${PROGRAM_NAME} ${PROJECT_VERSION}")
### LINUX PACKAGES / LIBRARIES ###
IF (LINUX)
    find_package(PkgConfig REQUIRED)
    # glib2
    pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
    # gdkpixbuf
    find_package(GDKPixBuf REQUIRED)
ENDIF()

######### SUBDIRS / LOCAL LIBRARIES #########
add_subdirectory("${LIBRARY_DIR}/blf")
add_subdirectory("${LIBRARY_DIR}/glad")
IF (WINDOWS_MSVC)
    add_subdirectory("${LIBRARY_DIR}/notificationcompat")
    ### WINDOWS 10 SDK ###
    # Use the env variable (not available with MinGW)
    IF (DEFINED ENV{WindowsSdkDir})
        # TODO: Get subdirectory (this only gets the /10/ dir, not the specific version)
        SET(WINDOWS_SDK_INCLUDE_DIR $ENV{WindowsSdkDir})
    ENDIF()
    # Try to find the SDK using a script
    IF (NOT WINDOWS_SDK_INCLUDE_DIR)
        include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/win-10-sdk-finder/find_win10_sdk.cmake" OPTIONAL)
    ENDIF()
    IF (WINDOWS_SDK_INCLUDE_DIR)
        # Pass the include directories to the notification compatibility library or it doesn't find the headers
        target_include_directories(notificationcompat PUBLIC "${WINDOWS_SDK_INCLUDE_DIR}/winrt")
        target_include_directories(notificationcompat PUBLIC "${WINDOWS_SDK_INCLUDE_DIR}/winrt/wrl")
        target_include_directories(notificationcompat PUBLIC "${WINDOWS_SDK_INCLUDE_DIR}/um")
        target_include_directories(notificationcompat PUBLIC "${WINDOWS_SDK_INCLUDE_DIR}/shared")
        message(STATUS "~~ Found Windows 10 SDK at: ${WINDOWS_SDK_INCLUDE_DIR}")
    ELSE()
        message(FATAL_ERROR "Could not find the Windows 10 SDK."
        "You can manually pass it with the cmake flag: -DWINDOWS_SDK_INCLUDE_DIR=<Path>")
    ENDIF()
ENDIF()

find_package(glfw3 REQUIRED)
add_subdirectory("${LIBRARY_DIR}/imgui") # imgui 1.90.4
target_link_libraries(imgui glfw) # imgui is dependent on glfw and causes a compile error if not linked with it here

######### DEFINES #########
### Build configuration ###
target_compile_definitions(${PROGRAM_NAME} PUBLIC "$<$<CONFIG:Debug>:DEBUG>")
target_compile_definitions(${PROGRAM_NAME} PUBLIC "$<$<NOT:$<CONFIG:Debug>>:NDEBUG>")
target_compile_definitions(${PROGRAM_NAME} PUBLIC "$<$<CONFIG:Release>:RELEASE>")

IF (ENABLE_TESTS)
    target_compile_definitions(${PROGRAM_NAME} PUBLIC "PERFORM_UNIT_TESTS")
ENDIF(ENABLE_TESTS)

######### COMPILE OPTIONS #########
add_library(compiler_flags INTERFACE)
# Add compile options defined by preset (might be empty)
target_compile_options(compiler_flags INTERFACE ${PRESET_ADD_COMPILE_OPTIONS})

message(STATUS "~~ Generator ${CMAKE_GENERATOR} on system ${CMAKE_SYSTEM_NAME}")
message(STATUS "~~ Using CXX compiler ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")
IF (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    message(STATUS "~~ Configuration: ${CMAKE_BUILD_TYPE}")
    IF (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    ENDIF()
    ###### WINDOWS OR MSYS SYSTEM ######
    IF (WIN32)
        ### COMPILER FLAGS ###
        # CLANG
        IF (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -static -fuse-ld=lld -mtune=native -lpthread -lcomtl32 -static-libstdc++ -std=${CPP_VERSION} -Wa,-mbig-obj")
        ENDIF()
        # GNU (G++)
        IF (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -static -fuse-ld=lld -mtune=native -lpthread -lcomctl32 -static-libstdc++ -static-libgcc -std=${CPP_VERSION} -Wa,-mbig-obj") # Pass  -Wa,-mbig-obj to avoid error for data_converter.cpp due to its size.
        ENDIF()
        message(STATUS "~~ Compile flags for MinGW: ${CMAKE_CXX_FLAGS}")

        ### LINKER FLAGS ###
        # /ENTRY:mainCRTStartup keeps the same "main" function instead of requiring "WinMain"
        IF ((${CMAKE_BUILD_TYPE} STREQUAL Debug) OR FORCE_CONSOLE)
            set(SUBSYSTEM_LINKER_OPTIONS "-Wl,-subsystem,console" "-Wl,-entry,mainCRTStartup")
        ELSEIF(${CMAKE_BUILD_TYPE} STREQUAL Release)
            set(SUBSYSTEM_LINKER_OPTIONS "-Wl,-subsystem,windows" "-Wl,-entry,mainCRTStartup")
        ENDIF()
        message(STATUS "~~ MinGW linker options: ${SUBSYSTEM_LINKER_OPTIONS}")
    ###### LINUX SYSTEM ######
    ELSEIF(LINUX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${CPP_VERSION} -lstdc++ -lnotify")
        message(STATUS "~~ Compile flags for Linux Makefiles: ${CMAKE_CXX_FLAGS}")
    ENDIF()
ELSEIF (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:${CPP_VERSION} /bigobj") # Pass /bigobj to avoid C1128 for data_converter.cpp due to its templates.
    message(STATUS "~~ Compile flags for Visual Studio: ${CMAKE_CXX_FLAGS}")
ENDIF()

######### LINK OPTIONS #########
target_link_options(${PROGRAM_NAME} PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})
IF (WINDOWS_MSVC)
    target_link_options(${PROGRAM_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<BOOL:${FORCE_CONSOLE}>>:/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup>
        $<$<AND:$<CONFIG:Release>,$<NOT:$<BOOL:${FORCE_CONSOLE}>>>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
    )
ENDIF()

######### INCLUDES #########
# Pass information like PROGRAM_NAME, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR and PROJECT_VERSION_PATCH to a header file to include
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/program_info.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/generated/program_info.h")

target_include_directories(${PROGRAM_NAME}
    PUBLIC "include" 
    PUBLIC "include/generated"
    PUBLIC "include/${CONSTANTS_DIR}"
    PUBLIC "include/${CONTAINERS_DIR}"
    PUBLIC "include/${HANDLERS_DIR}"
    PUBLIC "include/${INTERFACE_DIR}"
    PUBLIC "include/${IO_DIR}"
    PUBLIC "include/${SCHEDULE_DIR}"
    PUBLIC "include/${UTIL_DIR}"
    PUBLIC "libs"
    PUBLIC "libs/stb")

###### PLATFORM-SPECIFIC ######
IF (LINUX)
    IF (GDKPIXBUF_FOUND)
        target_include_directories(${PROGRAM_NAME} PUBLIC ${GDKPIXBUF_INCLUDE_DIR})
    ENDIF()
ENDIF()

######### SOURCES #########
target_sources(${PROGRAM_NAME} PRIVATE
    "${SCHETOOL_SRC}/main.cpp"
    "${SCHETOOL_SRC}/${CONSTANTS_DIR}/filter_constants.cpp"
    "${SCHETOOL_SRC}/${CONTAINERS_DIR}/date_container.cpp"
    "${SCHETOOL_SRC}/${CONTAINERS_DIR}/file_info.cpp"
    "${SCHETOOL_SRC}/${CONTAINERS_DIR}/select_container.cpp"
    "${SCHETOOL_SRC}/${CONTAINERS_DIR}/single_select_container.cpp"
    "${SCHETOOL_SRC}/${CONTAINERS_DIR}/select_options.cpp"
    "${SCHETOOL_SRC}/${CONTAINERS_DIR}/time_container.cpp"
    "${SCHETOOL_SRC}/${CONTAINERS_DIR}/time_wrapper.cpp"
    "${SCHETOOL_SRC}/${CONTAINERS_DIR}/weekday_container.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/gui_textures.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/input.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/notifications/notification_handler_linux_impl.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/notifications/notification_handler_win_impl.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/notifications/notification_handler.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/render.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/signals/signal_listener_linux_impl.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/signals/signal_listener_win_impl.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/signals/signal_listener.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/time_handler.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/window.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/autosave_popup_gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/edit_history_gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/schedule/element_editor_subgui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/schedule/filter_editor_subgui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/gui_templates.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/interface.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/interface_style.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/main_menu_bar/delete_modal_subgui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/main_menu_bar/main_menu_bar_gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/schedule/schedule_gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/start_page_gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/text_input_modal_subgui.cpp"
    "${SCHETOOL_SRC}/${IO_DIR}/io_handler.cpp"
    "${SCHETOOL_SRC}/${IO_DIR}/preferences_data_converter.cpp"
    "${SCHETOOL_SRC}/${IO_DIR}/preferences_io.cpp"
    "${SCHETOOL_SRC}/${IO_DIR}/schedule_data_converter.cpp"
    "${SCHETOOL_SRC}/${IO_DIR}/schedule_io.cpp"
    "${SCHETOOL_SRC}/${IO_DIR}/textures.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/element_base.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/filters/filter.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/filters/filter_group.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/filters/filter_rule_base.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule_column.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule_core.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule_edit.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule_edit_history.cpp"
    "${LIBRARY_DIR}/imgui/src/imgui_demo.cpp"
)

######### LINKS #########
target_link_libraries(${PROGRAM_NAME} PUBLIC compiler_flags 
    blf
    imgui
    glad 
    glfw
)

### PLATFORM-SPECIFIC ###
IF (WINDOWS_MSVC)
    target_link_libraries(${PROGRAM_NAME} PUBLIC notificationcompat)
    target_link_libraries(${PROGRAM_NAME} PUBLIC runtimeobject)
    target_link_libraries(${PROGRAM_NAME} PUBLIC comctl32)
ENDIF(WINDOWS_MSVC)

IF (LINUX)
    # glib 2.0
    target_link_libraries(${PROGRAM_NAME} PUBLIC PkgConfig::deps)
    IF (GDKPIXBUF_FOUND)
        target_link_libraries(${PROGRAM_NAME} PUBLIC ${GDKPIXBUF_LIBRARY})
    ENDIF()
ENDIF()

# Testing subdirectory
IF (ENABLE_TESTS)
    add_subdirectory(tests)
ENDIF(ENABLE_TESTS)

include("cmake/install.cmake")

include("cmake/package.cmake")