set(GENERATED_DIR  "${CMAKE_BINARY_DIR}/generated")
set(GENERATED_CONFIG_H  "${GENERATED_DIR}/config.generated.h")

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/config/config.h.in" "${GENERATED_CONFIG_H}"
)

include_directories(${GENERATED_DIR})
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/source/core)
add_subdirectory(core)

file(GLOB CONFIG_HEADERS config/*.h)
file(GLOB CONFIG_SOURCES config/*.cpp)

list(APPEND CONFIG_HEADERS ${GENERATED_CONFIG_H})

if(SSQT_BUILD_TUI)
    add_subdirectory(tui)
endif()

if(SSQT_BUILD_GUI)
    add_subdirectory(gui)
endif()
