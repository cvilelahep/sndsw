set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/shipLHC
${CMAKE_SOURCE_DIR}/analysis/tools
${CMAKE_SOURCE_DIR}/analysis/core
${CMAKE_SOURCE_DIR}/analysis/cuts
${CMAKE_SOURCE_DIR}/analysis/processes
)

include_directories( ${INCLUDE_DIRECTORIES} )

set(SRCS
sndSciFiHitFilter.cxx
sndSciFiHitFilter_legacy.cxx
sndCutFlowHist.cxx
)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
link_directories( ${LINK_DIRECTORIES})

Set(HEADERS )
Set(LINKDEF AnalysisProcsLinkDef.h)
Set(LIBRARY_NAME snd_analysis_procs)
Set(DEPENDENCIES snd_analysis_tools shipLHC veto)

GENERATE_LIBRARY()
