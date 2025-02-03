cmake_minimum_required(VERSION 3.25)

IF (NOT (("WIX" IN_LIST CPACK_GENERATOR) OR ("NSIS" IN_LIST CPACK_GENERATOR) OR ("NSIS64" IN_LIST CPACK_GENERATOR)))
    get_filename_component(PROJECT_BASE_DIR "${CPACK_OUTPUT_FILE_PATH}" DIRECTORY)
    MESSAGE(STATUS "Project base directory is: ${PROJECT_BASE_DIR}")
    get_filename_component(INSTALL_DIR "${CPACK_TEMPORARY_INSTALL_DIRECTORY}" DIRECTORY)
    MESSAGE(STATUS "Install directory is: ${INSTALL_DIR}")

    IF (INSTALL_DIR)
        # Get every subdirectory of the package directory
        file(GLOB folders LIST_DIRECTORIES TRUE
            "${INSTALL_DIR}/*"
        )

        list(LENGTH folders FOLDER_COUNT)
        # There should be exactly 1 subfolder - the install folder
        # If there are more or less than 1 it's unknown what we should do so just do nothing.
        IF (${FOLDER_COUNT} EQUAL 1)
            list(GET folders 0 INSTALL_FOLDER)
            file(MAKE_DIRECTORY "${INSTALL_FOLDER}/schedules")
        ENDIF()
    ENDIF()
ENDIF()