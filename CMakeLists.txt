cmake_minimum_required(VERSION 3.8)
project(Crawler)

option(DEBUG_MODE "enable debug mode" off)
if (DEBUG_MODE)
  add_definitions(-DDEBUG)
endif ()
## Configure debug flag to enable debug log

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")

set(CMAKE_CXX_STANDARD 17)
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/dom.cpp ${CMAKE_CURRENT_SOURCE_DIR}/html.cpp ${CMAKE_CURRENT_SOURCE_DIR}/http.cpp ${CMAKE_CURRENT_SOURCE_DIR}/json.cpp)
set(HEADERS ${HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}/dom.hpp ${CMAKE_CURRENT_SOURCE_DIR}/html.hpp ${CMAKE_CURRENT_SOURCE_DIR}/strings.hpp ${CMAKE_CURRENT_SOURCE_DIR}/test.hpp ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp)
add_executable(apptest ${SOURCES} test.cpp)
