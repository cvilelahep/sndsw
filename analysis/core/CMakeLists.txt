find_package(Boost REQUIRED COMPONENTS program_options)
find_package(FairLogger REQUIRED)



set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${FairLogger_LIBDIR}
)

link_directories( ${LINK_DIRECTORIES})

set(HEADERS)

add_executable(sndEventFilter sndEventFilter.cxx)
target_link_libraries(sndEventFilter ${ROOT_LIBRARIES} snd_analysis_procs snd_analysis_tools snd_analysis_cuts Boost::program_options -lFairLogger)
