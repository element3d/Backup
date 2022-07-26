cmake_minimum_required(VERSION 3.0)

set (PROJECT Backup)
project(${PROJECT})

include_directories(${CMAKE_SOURCE_DIR}/libs)
include_directories(${CMAKE_SOURCE_DIR}/modules)

add_library(SQLite3Cpp
    modules/SQLite3Cpp/SQLite3Cpp.cpp
    modules/SQLite3Cpp/SQLite3Cpp.h
	libs/sqlite3/sqlite3.c
)

add_executable(${PROJECT}
    main.cpp
)

target_link_libraries(${PROJECT} SQLite3Cpp)