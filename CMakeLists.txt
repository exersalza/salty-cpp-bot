cmake_minimum_required(VERSION 3.23)
project(dcbot VERSION 1.0 DESCRIPTION "First bot in cpp lulw")

set(CMAKE_CXX_STANDARD 20)

#[[add_subdirectory(flibs/DPP)]]

add_executable(dcbot src/main.cpp src/header/config.h src/source/config.cpp src/source/utils.cpp src/header/cogs.h src/cogs/adminCog.cpp)
include_directories(${Dpp_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} dpp)
target_include_directories(${PROJECT_NAME} PRIVATE
        #[[flibs/DPP/include]]
        )