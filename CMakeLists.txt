cmake_minimum_required(VERSION 3.19)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "You are configuring a 64bit build, this is not supported. Run cmake with `-A Win32`")
endif()

set(ProjectName "TestProject" CACHE STRING "")
mark_as_advanced(ProjectName)

message("ProjectName=${ProjectName}")

project(${ProjectName})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_compile_definitions("WIN32_LEAN_AND_MEAN")
add_compile_definitions(APP_NAME="${ProjectName}")
add_compile_definitions(APP_NAMEW=L"${ProjectName}")

add_library(${ProjectName} SHARED)

file(GLOB SOURCES
    "include/*.h"
    "src/*.h"
    "src/*.cpp")

target_sources(${ProjectName} PUBLIC ${SOURCES})
target_precompile_headers(${ProjectName} PRIVATE "src/pch.h")

target_compile_options(${ProjectName} PRIVATE /W4 /WX /ZI)

target_include_directories(${ProjectName} PUBLIC 
    "include/"
    "src/"
    )

include(directx)
include(imgui)
include(gwca)

target_link_libraries(${ProjectName} PRIVATE
    directx
    imgui    
    gwca)