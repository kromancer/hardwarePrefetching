cmake_minimum_required(VERSION 3.22)

project(hw_pref_control LANGUAGES C)

add_library(hw_pref_control STATIC msr.c pmu.c log.c)

target_include_directories(hw_pref_control PUBLIC .)
target_compile_options(hw_pref_control PRIVATE "-g" "-O0")
