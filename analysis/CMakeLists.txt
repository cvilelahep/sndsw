set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${VMC_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/shipdata
${CMAKE_SOURCE_DIR}/shipLHC
${CMAKE_SOURCE_DIR}/analysis/core
${CMAKE_SOURCE_DIR}/analysis/processes
${CMAKE_SOURCE_DIR}/analysis/cuts
${CMAKE_SOURCE_DIR}/analysis/tools
${FMT_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES} )

add_subdirectory(core)
add_subdirectory(processes)
add_subdirectory(cuts)
add_subdirectory(tools)
