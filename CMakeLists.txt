cmake_minimum_required(VERSION 3.15)
project(TextEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLTK REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(TextEditor WIN32 ${SOURCES})
target_include_directories(TextEditor PRIVATE ${CMAKE_SOURCE_DIR}/include ${FLTK_INCLUDE_DIR})
target_link_libraries(TextEditor PRIVATE ${FLTK_LIBRARIES})
