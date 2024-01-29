set(SND_CUTS_SRCS
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
)

set(LINK_DIRECTORIES
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

add_library(sndCuts SHARED ${SND_CUTS_SRCS})
target_link_libraries(sndCuts ${ROOT_LIBRARIES} -lROOTTPython sndTools shipLHC veto)
