cmake_minimum_required(VERSION 3.25)

project(schetool VERSION 0.8.0)

set(PROGRAM_NAME ${PROJECT_NAME})

set(CPP_VERSION "c++20")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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
message(STATUS "~~ Using CXX compiler ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")
IF (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    #### WINDOWS OR MSYS SYSTEM ####
    IF ((CMAKE_SYSTEM_NAME STREQUAL Windows) OR (CMAKE_SYSTEM_NAME STREQUAL MSYS))
        ### COMPILER FLAGS ###
        # CLANG
        IF (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -static -stdlib=libc++ -fuse-ld=lld -mtune=native -std=${CPP_VERSION}")
        ENDIF()
        # GNU (G++)
        IF (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -static -fuse-ld=lld -mtune=native -lpthread -lcomctl32 -static-libstdc++ -static-libgcc -std=${CPP_VERSION} -Wa,-mbig-obj") # Pass  -Wa,-mbig-obj to avoid error for data_converter.cpp due to its size.
        ENDIF()
        message(STATUS "~~ Set Unix Makefiles compile flags for Windows or MSYS system to: ${CMAKE_CXX_FLAGS}")

        ### WINDOWS LINKER FLAGS ###
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
    #### LINUX OR OTHER SYSTEM ####
    ELSE()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${CPP_VERSION} -lstdc++ -lnotify")
        message(STATUS "~~ Set Unix Makefiles base compile flags to: ${CMAKE_CXX_FLAGS}")
    ENDIF()
ELSEIF (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:${CPP_VERSION} /bigobj") # Pass /bigobj to avoid C1128 for data_converter.cpp due to its templates.
    message(STATUS "~~ Set Visual Studio compile flags to: ${CMAKE_CXX_FLAGS}")
ENDIF()
message(STATUS "~~ Finished setting compile flags")

find_package(glfw3 REQUIRED)

set(LIBRARY_DIR "libs")

# local source-compiled libraries
add_subdirectory("${LIBRARY_DIR}/blf")
add_subdirectory("${LIBRARY_DIR}/glad")
# NOTE: FSR 'WINDOWS' isn't set on MinGW
IF (WIN32 AND NOT MINGW)
    add_subdirectory("${LIBRARY_DIR}/notificationcompat")
ENDIF()

add_subdirectory("${LIBRARY_DIR}/imgui") # imgui 1.90.4
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

set(SCHETOOL_SRC    "source")
set(CONSTANTS_DIR   "constants")
set(CONTAINERS_DIR  "containers")
set(HANDLERS_DIR    "handlers")
set(INTERFACE_DIR   "interface")
set(IO_DIR          "io")
set(SCHEDULE_DIR    "schedule")
set(UTIL_DIR        "util")

# LINUX PACKAGES / LIBRARIES
IF (LINUX)
    find_package(PkgConfig REQUIRED)
    # glib2
    pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
    target_link_libraries(${PROGRAM_NAME} PUBLIC PkgConfig::deps)
    # gdkpixbuf
    find_package(GDKPixBuf REQUIRED)
    IF (GDKPIXBUF_FOUND)
        target_include_directories(${PROGRAM_NAME} PUBLIC ${GDKPIXBUF_INCLUDE_DIR})
        target_link_libraries(${PROGRAM_NAME} PUBLIC ${GDKPIXBUF_LIBRARY})
    ENDIF()
ENDIF()

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
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/notification_handler_linux_impl.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/notification_handler_win_impl.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/notification_handler.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/render.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/signal_handler_linux_impl.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/signal_handler_win_impl.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/signal_handler.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/textures.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/time_handler.cpp"
    "${SCHETOOL_SRC}/${HANDLERS_DIR}/window.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/autosave_popup_gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/edit_history_gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/element_editor_subgui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/filter_editor_subgui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/gui_templates.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/interface.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/interface_style.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/main_menu_bar_gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/schedule_gui.cpp"
    "${SCHETOOL_SRC}/${INTERFACE_DIR}/start_page_gui.cpp"
    "${SCHETOOL_SRC}/${IO_DIR}/data_converter.cpp"
    "${SCHETOOL_SRC}/${IO_DIR}/io_handler.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/element_base.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/filter.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/filter_group.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/filter_rule_base.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule_column.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule_core.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule_edit.cpp"
    "${SCHETOOL_SRC}/${SCHEDULE_DIR}/schedule_edit_history.cpp"
    "${LIBRARY_DIR}/imgui/src/imgui_demo.cpp"
)

# PLATFORM-SPECIFIC INCLUDE DIRS
IF (WIN32 AND NOT MINGW)
    #### WINDOWS 10 SDK ####
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

target_link_libraries(${PROGRAM_NAME} PUBLIC compiler_flags 
    blf
    imgui
    glad 
    glfw
)

# PLATFORM-SPECIFIC LINKS
IF (WIN32)
    IF (NOT MINGW)
        target_link_libraries(${PROGRAM_NAME} PUBLIC notificationcompat)
        target_link_libraries(${PROGRAM_NAME} PUBLIC runtimeobject)
        target_link_libraries(${PROGRAM_NAME} PUBLIC comctl32)
    ENDIF(NOT MINGW)
ENDIF(WIN32)

# add testing subdirectory (handles its own logic)
add_subdirectory(tests)


########## INSTALL ##########
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
install(TARGETS ${PROGRAM_NAME}
    DESTINATION "."
    RUNTIME_DEPENDENCY_SET schetool_deps
)
install(FILES "README.md"
    DESTINATION "."
)
install(DIRECTORY "fonts"
    DESTINATION "."
)
install(DIRECTORY "textures"
    DESTINATION "."
)
# This will be removed after packaging for WiX packages
install(CODE [[
    file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/schedules")
]])
IF (WIN32)
    install(FILES "$<TARGET_FILE:${PROGRAM_NAME}>/../glfw3.dll"
        DESTINATION "."
    )
ENDIF(WIN32)

set(PACKAGING_DIR "${CMAKE_SOURCE_DIR}/packaging")

########## PACKAGING ##########
IF (WIN32 AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # WIX PACKAGE COMMAND:
    # cpack -G WIX -C [Release | Debug] --config build\CPackConfig.cmake
    # NOTE: Any settings that CPack uses must be defined before including it
    # NOTE: Some package generators support components to optionally install (might be needed in the future)
    # cpack_add_component
    set(CPACK_PACKAGE_NAME "${PROGRAM_NAME}")
    set(CPACK_PACKAGE_VENDOR "Kip (KipHub)")
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROGRAM_NAME}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A lightweight daily schedule making program.")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${PACKAGING_DIR}/package_description.txt")
    # set(CPACK_WIX_LICENSE_RTF) TODO: Add license
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/textures/icon.ico")
    set(CPACK_VERBATIM_VARIABLES TRUE)
    # TODO: Look into these to see which ones should be constant
    # Currently, if schetool is already installed, the installer will fail no matter what (i believe)
    set(CPACK_WIX_PRODUCT_GUID "79A832A4-47BC-46CD-998A-73DCD7CAF255")
    set(CPACK_WIX_UPGRADE_GUID "79A832A4-47BC-46CD-998A-73DCD7CAF255")
    # The name of the folder in start menu containing the exe, uninstaller and maybe other things
    set(CPACK_WIX_PROGRAM_MENU_FOLDER "${PROGRAM_NAME}")
    ## Add or Remove Programs ##
    # The description shown when the program is selected in 'Add or Remove Programs'
    set(CPACK_WIX_PROPERTY_ARPCOMMENTS "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
    # Program about info link (for now just a link to its github)
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://github.com/Kippye/schetool")
    ## Shortcuts ##
    # Start menu and desktop shortcuts
    set(CPACK_PACKAGE_EXECUTABLES "${PROGRAM_NAME};${PROGRAM_NAME}")
    set(CPACK_CREATE_DESKTOP_LINKS "${PROGRAM_NAME};${PROGRAM_NAME}")

    # WIX POST PACKAGE SCRIPT
    # Deletes the MSI installer and builds a new one to replace it
    # Adds ShortcutProperty elements to the start menu shortcut in files.wxs
    # Also removes the "schedules" dir from install for WiX (modifies files.wxs and features.wxs and rebuilds their .wixobj files)
    # NOTE: Does nothing for other package generators
    set(CPACK_POST_BUILD_SCRIPTS "${PACKAGING_DIR}/fix_wix.cmake")

    # We don't want to split our program up into several things
    # TODO: Actually look up what this does
    set(CPACK_MONOLITHIC_INSTALL 1)
    include(CPack)
ENDIF()