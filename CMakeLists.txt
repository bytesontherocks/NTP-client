cmake_minimum_required(VERSION 3.10)

project(ntp_client_lib)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glog CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp include/*.h*)
include_directories(include)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_executable(ntp_example main.cpp)
target_link_libraries(ntp_example ${PROJECT_NAME} glog::glog)

add_subdirectory(test)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif()

INSTALL(TARGETS ${PROJECT_NAME}
        LIBRARY
        DESTINATION bin)

install(DIRECTORY "include/"
    DESTINATION "include"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)