cmake_minimum_required(VERSION 3.8)
project(doubanCrawler)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/dom.cpp ${CMAKE_CURRENT_SOURCE_DIR}/html.cpp ${CMAKE_CURRENT_SOURCE_DIR}/http.cpp)
set(HEADERS ${HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}/dom.hpp ${CMAKE_CURRENT_SOURCE_DIR}/html.hpp ${CMAKE_CURRENT_SOURCE_DIR}/strings.hpp ${CMAKE_CURRENT_SOURCE_DIR}/test.hpp)
add_executable(http http.cpp)
add_executable(main ${HEADERS} ${SOURCES})
add_executable(test ${HEADERS} test.cpp)
