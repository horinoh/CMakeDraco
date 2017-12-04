cmake_minimum_required(VERSION 3.6.1)

set(ProjectName DRACO)
project(${ProjectName})

include_directories(..//..//draco//src)
link_directories(..//..//draco//build)
link_libraries(draco dracodec dracoenc)
#add_definitions()

# Windows ビルド用に予め用意したファイル
# ReadMe.txt
# Resource.h
# small.ico
# stdafx.cpp, stdafx.h
# targetver.h
# VK.ico, VK.rc
file(GLOB_RECURSE SourceFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HeaderFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE ResourceFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.rc)
file(GLOB_RECURSE IconFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.ico)
add_executable(${ProjectName} ${SourceFiles} ${HeaderFiles} ${ResourceFiles} ${IconFiles})

set_target_properties(${ProjectName} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")