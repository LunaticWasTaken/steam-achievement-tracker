cmake_minimum_required(VERSION 3.31)
project(lntc_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets)
install(DIRECTORY include/ DESTINATION include)

if (EXISTS "${CMAKE_SOURCE_DIR}/test")
    file(GLOB_RECURSE TEST_HEADERS "${CMAKE_SOURCE_DIR}/test/*.hpp")
    file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")
    add_executable(${PROJECT_NAME}_test ${TEST_HEADERS} ${TEST_SOURCES})
    unset(TEST_HEADERS)
    unset(TEST_SOURCES)
endif ()
