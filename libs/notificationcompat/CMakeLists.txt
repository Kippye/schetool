cmake_minimum_required(VERSION 3.25)
project(notificationcompat VERSION 1.0.0 LANGUAGES CXX)

set(NOTIFCOMPAT_LIBNAME notificationcompat)
add_library(${NOTIFCOMPAT_LIBNAME} STATIC DesktopNotificationManagerCompat.h DesktopNotificationManagerCompat.cpp)
target_include_directories(${NOTIFCOMPAT_LIBNAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(${NOTIFCOMPAT_LIBNAME}
    PRIVATE UNICODE
    PRIVATE _UNICODE
)