cmake_minimum_required(VERSION 3.8)

project(glad)

set(GLAD_SRC ./src)

add_library(glad STATIC 
${GLAD_SRC}/glad.c
)

target_include_directories(glad PUBLIC include)