cmake_minimum_required(VERSION 3.8)
project(doubanCrawler)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/dom.cpp ${CMAKE_CURRENT_SOURCE_DIR}/html.cpp ${CMAKE_CURRENT_SOURCE_DIR}/http.cc)
set(HEADERS ${HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}/dom.h ${CMAKE_CURRENT_SOURCE_DIR}/html.h ${CMAKE_CURRENT_SOURCE_DIR}/strings.h ${CMAKE_CURRENT_SOURCE_DIR}/test.h)
add_executable(http http.cc)
add_executable(main ${HEADERS} ${SOURCES})
add_executable(test ${HEADERS} test.cpp)
