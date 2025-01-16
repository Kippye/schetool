IF ("WIX" IN_LIST CPACK_GENERATOR)
    get_filename_component(PROJECT_BASE_DIR "${CPACK_OUTPUT_FILE_PATH}" DIRECTORY)
    MESSAGE(STATUS "Project base directory is: ${PROJECT_BASE_DIR}")
    get_filename_component(WIX_DIR "${CPACK_TEMPORARY_INSTALL_DIRECTORY}" DIRECTORY)
    MESSAGE(STATUS "WIX directory is: ${WIX_DIR}")

    IF (NOT WIX_DIR)
        set(WIX_DIR "${PROJECT_BASE_DIR}/_CPACK_PACKAGES/win64/WIX")
        MESSAGE(STATUS "WIX dir was not found directly, created from PROJECT_BASE_DIR: ${WIX_DIR}")
    ENDIF(NOT WIX_DIR)
    IF (EXISTS "${WIX_DIR}/files.wxs" AND EXISTS "${WIX_DIR}/features.wxs")
        # Python is currently required to "fix" 'files.wxs'
        find_package(Python3 REQUIRED)
        # Wix is also required (duh)
        find_package(WIX REQUIRED)
        
        file(GLOB PREVIOUS_MSI_FILES
            "${WIX_DIR}/*.msi"
        )
        list(LENGTH PREVIOUS_MSI_FILES EXISTING_MSI_COUNT)
        # We can only replace CPack's MSI installer if there was one in the first place
        # Otherwise, the installer's file name would have to be hardcoded here
        IF (${EXISTING_MSI_COUNT} GREATER 0)
            # Delete the MSI installer created by CPack
            file(REMOVE ${PREVIOUS_MSI_FILES})
            # Fix the wxs files created by CPack
            # This is required until another way to insert ShortcutProperty elements into the Start Menu Shortcut of 'files.wxs' is found
            # Also to remove the 'schedules' folder from the installation while keeping it for ZIP packages
            execute_process(
                COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_LIST_DIR}/fix_wix.py" "${WIX_DIR}/files.wxs" "${WIX_DIR}/features.wxs")
            # Call candle to build files.wixobj
            execute_process(
                COMMAND "${WIX_CANDLE}" -nologo -sw -arch x64 -out "${WIX_DIR}/files.wixobj" "-I${WIX_DIR}" "${WIX_DIR}/files.wxs"
                WORKING_DIRECTORY "${WIX_DIR}"
            )
            # Call candle to build features.wixobj
            execute_process(
                COMMAND "${WIX_CANDLE}" -nologo -sw -arch x64 -out "${WIX_DIR}/features.wixobj" "-I${WIX_DIR}" "${WIX_DIR}/features.wxs"
                WORKING_DIRECTORY "${WIX_DIR}"
            )
            list(GET PREVIOUS_MSI_FILES 0 NEW_MSI_PATH)
            # Call light to link wixobj files into an MSI installer with the same name as the one created by CPack
            execute_process(
                COMMAND "${WIX_LIGHT}" -nologo -sw -out "${NEW_MSI_PATH}" -ext "WixUIExtension"  "${WIX_DIR}/directories.wixobj" "${WIX_DIR}/files.wixobj" "${WIX_DIR}/features.wixobj" "${WIX_DIR}/main.wixobj"
                WORKING_DIRECTORY "${WIX_DIR}"
            )
            MESSAGE(STATUS "Replaced CPack's MSI installer with a fixed one.")
        ENDIF(${EXISTING_MSI_COUNT} GREATER 0)
    ENDIF()
ENDIF("WIX" IN_LIST CPACK_GENERATOR)