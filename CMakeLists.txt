cmake_minimum_required(VERSION 3.23)
project(dcbot VERSION 1.0 DESCRIPTION "First bot in cpp lulw")

set(CMAKE_CXX_STANDARD 20)

#[[add_subdirectory(flibs/DPP)]]

add_executable(dcbot src/main.cpp src/header/config.hpp src/source/config.cpp src/source/utils.cpp src/header/cogs.hpp src/cogs/adminCog.cpp src/cogs/economyCog.cpp src/header/ticket.hpp src/source/ticket.cpp)
include_directories(${Dpp_INCLUDE_DIRS})

find_package(fmt REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(${PROJECT_NAME} dpp fmt nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PRIVATE
        #[[flibs/DPP/include]]
        )