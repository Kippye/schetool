######### PACKAGING #########
cmake_minimum_required(VERSION 3.25)

IF (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
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
    IF (WIN32)
        # set(CPACK_WIX_LICENSE_RTF) TODO: Add license
        set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/textures/icon.ico")
        set(CPACK_VERBATIM_VARIABLES TRUE)
        # UPDATE this GUID whenever a new version of the installer is made so that it can replace the old installation
        set(CPACK_WIX_PRODUCT_GUID "E2267656-4703-4A40-B337-AD89B3AA2442")
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
        
        # WIX POST PACKAGE SCRIPT
        # Deletes the MSI installer and builds a new one to replace it
        # Adds ShortcutProperty elements to the start menu shortcut in files.wxs
        # NOTE: Does nothing for other package generators
        set(CPACK_POST_BUILD_SCRIPTS "${PACKAGING_DIR}/fix_wix.cmake")
    ENDIF(WIN32)

    # We don't want to split our program up into several things
    # TODO: Actually look up what this does
    set(CPACK_MONOLITHIC_INSTALL 1)
    include(CPack)
ENDIF()