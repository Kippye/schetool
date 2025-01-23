######### PACKAGING #########
cmake_minimum_required(VERSION 3.25)

IF (WIN32 AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # WIX PACKAGE COMMAND:
    # cpack -G WIX -C [Release | Debug] --config build\CPackConfig.cmake
    # NOTE: Any settings that CPack uses must be defined before including it
    # NOTE: Some package generators support components to optionally install (might be needed in the future): cpack_add_component
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
    # UPDATE this GUID whenever a new version of the installer is made so that it can replace the old installation
    set(CPACK_WIX_PRODUCT_GUID "1B3CDF17-DBB9-4E74-A598-C8329E864679")
    set(CPACK_WIX_UPGRADE_GUID "79A832A4-47BC-46CD-998A-73DCD7CAF255")
    # The name of the folder in start menu containing the exe, uninstaller and maybe other things
    set(CPACK_WIX_PROGRAM_MENU_FOLDER "${PROGRAM_NAME}")
    ### Add or Remove Programs ###
    # The description shown when the program is selected in 'Add or Remove Programs'
    set(CPACK_WIX_PROPERTY_ARPCOMMENTS "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
    # Program about info link (for now just a link to its github)
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://github.com/Kippye/schetool")
    ### Shortcuts ###
    # Start menu and desktop shortcuts
    set(CPACK_PACKAGE_EXECUTABLES "${PROGRAM_NAME};${PROGRAM_NAME}")
    set(CPACK_CREATE_DESKTOP_LINKS "${PROGRAM_NAME};${PROGRAM_NAME}")

    # Currently just adds a "schedules" subdirectory for archive packages (ZIP, 7z, etc)
    # Because i think when you extract a program from a ZIP file you don't expect it to go adding files to your system
    set(CPACK_INSTALL_SCRIPTS "${PACKAGING_DIR}/generator_specific_install.cmake")
    # WIX POST PACKAGE SCRIPT
    # Deletes the MSI installer and builds a new one to replace it
    # Adds ShortcutProperty elements to the start menu shortcut in files.wxs
    # NOTE: Does nothing for other package generators
    set(CPACK_POST_BUILD_SCRIPTS "${PACKAGING_DIR}/fix_wix.cmake")

    # We don't want to split our program up into several things
    # TODO: Actually look up what this does
    set(CPACK_MONOLITHIC_INSTALL 1)
    include(CPack)
ENDIF()