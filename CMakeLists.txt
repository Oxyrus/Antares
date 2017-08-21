cmake_minimum_required(VERSION 3.8)
project(Antares)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/)

set(CMAKE_CXX_STANDARD 17)

set(SFML_ROOT ${PROJECT_SOURCE_DIR}/sfml/)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

# Find SFML
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML REQUIRED COMPONENTS audio graphics window system)

if(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML couldn't be located!")
endif()

include_directories(${SFML_INCLUDE_DIR})

set(SOURCE_FILES main.cpp)
add_executable(Antares ${SOURCE_FILES})

target_link_libraries(Antares ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
