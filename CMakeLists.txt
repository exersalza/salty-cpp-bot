cmake_minimum_required(VERSION 3.23)
project(dcbot VERSION 1.0 DESCRIPTION "First bot in cpp lulw")

set(CMAKE_CXX_STANDARD 20)

#[[add_subdirectory(flibs/DPP)]]
aux_source_directory(src main)
aux_source_directory(src/source source)
aux_source_directory(src/include include)

add_executable(${PROJECT_NAME} ${main} ${source} ${include} src/source/user.cpp src/include/user.hpp)

include_directories(${Dpp_INCLUDE_DIRS})
include_directories(/usr/include/mysql/)
include_directories(libs/)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/usr/local/lib")

find_package(fmt REQUIRED)

target_link_libraries(${PROJECT_NAME} dpp fmt mysqlpp curl)
target_include_directories(${PROJECT_NAME} PRIVATE
        flibs/DPP/include
        flibs/libcurl-openssl-1.0
        flibs/mysql++/include)
