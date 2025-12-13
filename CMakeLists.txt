cmake_minimum_required(VERSION 3.16)

project(NoteThat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

add_executable(notethat
    src/main.cpp
)

target_include_directories(notethat PRIVATE ${GTKMM_INCLUDE_DIRS})
target_link_libraries(notethat PRIVATE ${GTKMM_LIBRARIES})
target_compile_options(notethat PRIVATE ${GTKMM_CFLAGS_OTHER})
