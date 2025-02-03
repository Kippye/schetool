message(STATUS "~~ Testing is enabled.")
set(TEST_MAIN "unit_tests")
set(TEST_SOURCES "main.cpp")

# package libraries
#find_package(Catch2 3)
# TEMP
add_subdirectory(../libs/catch2 ../catch2)
#IF (Catch2_FOUND)
    #message(CHECK_PASS "found")
    add_executable(${TEST_MAIN} ${TEST_SOURCES})

    target_link_libraries(${TEST_MAIN} PUBLIC schetool Catch2::Catch2WithMain)
    add_test(NAME ${TEST_MAIN} COMMAND ${TEST_MAIN})
#ELSE()
    #message(CHECK_FAIL "not found. Tests will not be included.")
#ENDIF(Catch2_FOUND)