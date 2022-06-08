include_guard()
include(FetchContent)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git)
FetchContent_GetProperties(gwca)

if (minhook_POPULATED)
    return()
endif()

FetchContent_Populate(minhook)
add_library(minhook)

file(GLOB SOURCES
    "${minhook_SOURCE_DIR}/include/*.h"
    "${minhook_SOURCE_DIR}/src/*.h"
    "${minhook_SOURCE_DIR}/src/*.c"
    "${minhook_SOURCE_DIR}/src/hde/*.h"
    "${minhook_SOURCE_DIR}/src/hde/*.c"
    )

source_group(TREE "${minhook_SOURCE_DIR}" FILES ${SOURCES})
target_sources(minhook PRIVATE ${SOURCES})

target_include_directories(minhook PUBLIC "${minhook_SOURCE_DIR}/include/")
