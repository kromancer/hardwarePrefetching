cmake_minimum_required(VERSION 3.22)

project(hwpref_ctrl LANGUAGES C)

add_library(hwpref_ctrl SHARED msr.c pmu.c log.c)

target_include_directories(hwpref_ctrl PUBLIC .)
target_compile_options(hwpref_ctrl PRIVATE "-g" "-O0")
