# File - Delete cache してから 「Visual Studio のバージョンと x64 等を選択して」 Generate する

cmake_minimum_required(VERSION 3.6.1)

project(MyProject)

#message("CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
#message("CMAKE_CXX_FLAGS_DEBUG = " ${CMAKE_CXX_FLAGS_DEBUG})
#message("CMAKE_CXX_FLAGS_RELEASE = " ${CMAKE_CXX_FLAGS_RELEASE})

add_subdirectory(DRACO)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DRACO)
