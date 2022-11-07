cmake_minimum_required(VERSION 3.23)
project(dcbot VERSION 1.0 DESCRIPTION "First bot in cpp lulw")

set(CMAKE_CXX_STANDARD 20)

#[[add_subdirectory(flibs/DPP)]]

add_executable(dcbot src/main.cpp src/include/config.hpp src/source/config.cpp src/source/utils.cpp src/include/cogs.hpp src/include/economyCogs.hpp src/source/ticketCogs.cpp src/include/ticketCogs.hpp src/source/economyCogs.cpp src/source/cogs.cpp)
include_directories(${Dpp_INCLUDE_DIRS})
include_directories("/usr/local/lib")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/usr/local/lib")

find_package(fmt REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(${PROJECT_NAME} dpp fmt sqlparser nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PRIVATE
        #[[flibs/DPP/include]]
        )