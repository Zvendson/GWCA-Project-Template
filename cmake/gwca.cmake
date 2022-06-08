include_guard()
include(FetchContent)

FetchContent_Declare(
    gwca
    GIT_REPOSITORY https://github.com/GregLando113/GWCA.git)
FetchContent_GetProperties(gwca)

if (gwca_POPULATED)
    return()
endif()

FetchContent_Populate(gwca)
add_library(gwca)

file(GLOB SOURCES
    "${gwca_SOURCE_DIR}/source/stdafx.h"
    "${gwca_SOURCE_DIR}/source/*.cpp"
    "${gwca_SOURCE_DIR}/include/gwca/constants/*.h"
    "${gwca_SOURCE_DIR}/include/gwca/context/*.h"
    "${gwca_SOURCE_DIR}/include/gwca/gamecontainers/*.h"
    "${gwca_SOURCE_DIR}/include/gwca/gameentities/*.h"
    "${gwca_SOURCE_DIR}/include/gwca/managers/*.h"
    "${gwca_SOURCE_DIR}/include/gwca/packets/*.h"
    "${gwca_SOURCE_DIR}/include/gwca/utilities/*.h")

source_group(TREE "${gwca_SOURCE_DIR}" FILES ${SOURCES})
target_sources(gwca PRIVATE ${SOURCES})

target_precompile_headers(gwca PRIVATE "${gwca_SOURCE_DIR}/source/stdafx.h")
target_include_directories(gwca PUBLIC "${gwca_SOURCE_DIR}/include/")

include(minhook)

target_link_libraries(gwca PUBLIC minhook)