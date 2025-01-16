# CMake
# It is not modularized.

if(${MINGW})
    if(NOT WINDOWS_SDK_INCLUDE_DIR)
        get_filename_component(WINDOWS_SDK_ROOT_DIR
            "[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" REALPATH)
        file(GLOB WINDOWS_SDK_INCS LIST_DIRECTORIES true "${WINDOWS_SDK_ROOT_DIR}/Include/10.0.*.*")
        list(SORT WINDOWS_SDK_INCS ORDER DESCENDING)
        list(GET WINDOWS_SDK_INCS 0 WINDOWS_SDK_INCLUDE_DIR)
    endif()
    if(NOT EXISTS ${WINDOWS_SDK_INCLUDE_DIR})
        message(FATAL_ERROR "Could not find the directory; ${WINDOWS_SDK_INCLUDE_DIR}."
            "You can fix by the cmake flag; -DWINDOWS_SDK_INCLUDE_DIR=<Path>")
    endif()
endif()
