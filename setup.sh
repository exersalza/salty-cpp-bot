#!/bin/bash

echo "Creating dirs"

mkdir -p "${PWD}/src/source"
mkdir -p "${PWD}/src/include"
mkdir -p "${PWD}/src/resources"

echo "Creating files"

cat > "${PWD}/src/main.cpp" <<- EOM
#include <dpp/dpp.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  return 0;
}
EOM

cat > CMakeLists.txt <<- EOM
cmake_minimum_required(VERSION 3.23)
project(PORJECT_NAME VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

add_executable(PROJECT_NAME src/main.cpp)
include_directories(${Dpp_INCLUDE_DIRS})

find_package(fmt REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(\${PROJECT_NAME} dpp fmt nlohmann::nlohmann)
target_include_directories(\${PROJECT_NAME} PRIVATE)
EOM
