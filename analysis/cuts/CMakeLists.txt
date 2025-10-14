set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${VMC_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/shipdata
${CMAKE_SOURCE_DIR}/genfit/core/include
${CMAKE_SOURCE_DIR}/shipLHC
${CMAKE_SOURCE_DIR}/analysis/processes
${CMAKE_SOURCE_DIR}/analysis/cuts
${CMAKE_SOURCE_DIR}/analysis/core
${FMT_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES} )

set(SRCS
sndSciFiBaseCut.cxx
sndMuFilterBaseCut.cxx
sndMinSciFiHitsCut.cxx
sndSciFiStationCut.cxx
sndVetoCut.cxx
sndMinSciFiConsecutivePlanes.cxx
sndDSActivityCut.cxx
sndUSQDCCut.cxx
sndEventHeaderBaseCut.cxx
sndEventDeltat.cxx
sndAvgSciFiFiducialCut.cxx
sndAvgDSFiducialCut.cxx
sndDSVetoCut.cxx
sndSciFiContinuity.cxx
sndUSPlanesHit.cxx
sndUSBarsVeto.cxx
sndSciFiAngle.cxx
sndStableBeams.cxx
sndIP1.cxx
sndPrescale.cxx
sndNtrackCut.cxx
sndEventDirectionCut.cxx
sndTrackFiducialCut.cxx
sndTrackSciFiDOCA.cxx
sndMaxDShits.cxx
)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

Set(HEADERS )
Set(LINKDEF AnalysisCutsLinkDef.h)
Set(LIBRARY_NAME snd_analysis_cuts)
Set(DEPENDENCIES snd_analysis_tools shipLHC veto -lROOTTPython genfit)

GENERATE_LIBRARY()
