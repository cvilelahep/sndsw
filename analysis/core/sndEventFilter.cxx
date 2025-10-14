#include <string>
#include <iostream>
#include <vector>

#include <filesystem>

#include <boost/program_options.hpp>

#include "FairLogger.h"

#include "TROOT.h"
#include "TInterpreter.h"
#include "TGlobal.h"
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1D.h"
#include "TPython.h"

#include "ShipMCTrack.h"
#include "SNDLHCEventHeader.h"
#include "Scifi.h"
#include "MuFilter.h"

#include "sndFilterProcessBase.h"
#include "sndBaseCut.h"

namespace po = boost::program_options;

int main(int argc, char ** argv) {

  fair::Logger::OnFatal([](){
    TPython::Exec("try:\n  sndgeo\nexcept NameError:\n  pass\nelse:\n  del sndgeo");
    throw std::runtime_error("Fatal error");
  });
  
  po::options_description desc("SNDLHC event filter");
  desc.add_options()
    ("help,h", "Show help message")
    ("input,i", po::value<std::string>()->required(), "Input file name (or reg exp)")
    ("output,o", po::value<std::string>()->required(), "Output file name")
    ("geofile,g", po::value<std::string>()->required(), "Geometry file name")
    ("pipeline,p", po::value<std::string>()->required(), "Path to process pipeline definition. For example, in ${SNDSW_ROOT}/analysis/pipelines/")
    ("recofile,r", po::value<std::string>()->default_value(""), "Path to reconstructed muon tracks file.");
  
  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    std::cout << desc << std::endl;
    return 1;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }

  std::string input = vm["input"].as<std::string>();
  std::string output = vm["output"].as<std::string>();
  std::string geofile = vm["geofile"].as<std::string>();
  std::string pipeline_filename = vm["pipeline"].as<std::string>();
  std::string recofile = vm["recofile"].as<std::string>();
  
  // Check if files exist
  if (! std::filesystem::exists(geofile)) {
    LOG(FATAL) << "Geometry file does not exist: "+geofile; 
  }
  if (! std::filesystem::exists(pipeline_filename)) {
    LOG(FATAL) << "Cut set definition file does not exist: "+pipeline_filename;
  }
  
  // Input files
  bool isMC = false;
  TChain * ch = new TChain("rawConv");
  ch->Add(input.c_str());
  if (ch->GetEntries() == 0){
    delete ch;
    ch = new TChain("cbmsim");
    ch->Add(input.c_str());
    if (ch->GetEntries() > 0) {
      isMC = true;
    } else {
      LOG(FATAL) << "Didn't find rawConv or cbmsim in input file";
    }
  }
  LOG(INFO) << "Got input tree";

  TChain * ch_reco;
  if (!recofile.empty()){
    if (isMC) ch_reco = new TChain("cbmsim");
    else ch_reco = new TChain("rawConv");
    ch_reco->Add(recofile.c_str());
    ch->AddFriend(ch_reco);
  }

  // Use ROOT list of globals to share the event data with the cut classes
  // Add TTree to ROOT globals
  TCollection * rootGlobals = gROOT->GetListOfGlobals();
  rootGlobals->Add(ch);

  // Add branches to ROOT globals
  // EventHeader
  SNDLHCEventHeader * event_header = new SNDLHCEventHeader();
  ch->SetBranchAddress("EventHeader", &event_header);
  ch->GetEntry(0);
  if (event_header->GetEventNumber() == -1) {
    ch->SetBranchAddress("EventHeader.", &event_header);
    ch->GetEntry(0);
    if (event_header->GetEventNumber() == -1) {
      LOG(FATAL) << "Invalid event header"; 
    }
  }
  rootGlobals->Add(event_header);

  // SciFi DigiHits
  TClonesArray * scifiDigiHitCollection = new TClonesArray("sndScifiHit", 3000);
  ch->SetBranchAddress("Digi_ScifiHits", &scifiDigiHitCollection);
  rootGlobals->Add(scifiDigiHitCollection); // Name: sndScifiHits

  // MuFilter DigiHits
  TClonesArray * muFilterDigiHitCollection = new TClonesArray("MuFilterHit", 470);
  ch->SetBranchAddress("Digi_MuFilterHits", &muFilterDigiHitCollection);
  rootGlobals->Add(muFilterDigiHitCollection); // Name: MuFilterHits
  
  // MC truth
  TClonesArray * MCTracks; 
  if (isMC) {
    MCTracks = new TClonesArray("ShipMCTrack", 5000);
    ch->SetBranchAddress("MCTrack", &MCTracks);
    rootGlobals->Add(MCTracks); // Name: ShipMCTracks
  }

  TClonesArray * Reco_MuonTracks;
  if (!recofile.empty()){
    Reco_MuonTracks = new TClonesArray("sndRecoTrack", 10);
    ch->SetBranchAddress("Reco_MuonTracks", &Reco_MuonTracks);
    rootGlobals->Add(Reco_MuonTracks); // Name: sndRecoTracks
  }
  
  // SNDSW geometry:
  TPython::Exec("import SndlhcGeo");
  TPython::Exec(("sndgeo = SndlhcGeo.GeoInterface('"+geofile+"')").c_str());
  // Hacky hacky
  TPython::Exec("import atexit");
  TPython::Exec("def pyExit():\n print(\"Make suicide until solution found for freezing\")\n os.system('kill '+str(os.getpid()))\natexit.register(pyExit)");

  // Get detector objects
  Scifi * scifiDet = dynamic_cast<Scifi*>(gROOT->GetListOfGlobals()->FindObject("Scifi"));
  if (! scifiDet){
    LOG(FATAL) << "SciFi detector object not found. Exitting.";
  }
  MuFilter * mufilterDet = dynamic_cast<MuFilter*>(gROOT->GetListOfGlobals()->FindObject("MuFilter"));
  if (! mufilterDet){
    LOG(FATAL) << "MuFilter detector object not found. Exitting.";
  }
  
  // Output file
  TFile * outFile = new TFile(output.c_str(), "RECREATE");
  LOG(INFO) << "Got output file";

  // Get the pipeline via the ROOT interpreter
  gInterpreter->Declare(("#include \""+pipeline_filename+"\"").c_str());

  if (! static_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("pipeline"))){
    LOG(FATAL) << "Invalid pipeline file";
  }
  std::vector< snd::analysis_core::baseProcess * > * pipeline_ptr = static_cast<std::vector< snd::analysis_core::baseProcess * > *>(static_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("pipeline"))->GetAddress());

  std::vector< snd::analysis_core::baseProcess * >& pipeline = *pipeline_ptr;

  LOG(INFO) << "================================================================================";
  LOG(INFO) << "PIPELINE:";
  for (snd::analysis_core::baseProcess * cut : pipeline) LOG(INFO) << cut->getName();
  LOG(INFO) << "================================================================================";
  
  ch->GetEntry(0);
  ch->GetFile()->Get("BranchList")->Write("BranchList", TObject::kSingleKey);
  ch->GetFile()->Get("TimeBasedBranchList")->Write("TimeBasedBranchList", TObject::kSingleKey);
  if (ch->GetFile()->Get("FileHeader")) ch->GetFile()->Get("FileHeader")->Write();
  if (ch->GetFile()->Get("FileHeaderHeader")) ch->GetFile()->Get("FileHeaderHeader")->Write();

  // Set up all branches to copy to output TTree.
  TTree * outTree = ch->CloneTree(0);
  LOG(INFO) << "Got output tree";
  
  // Get number of entries
  unsigned long int n_entries = ch->GetEntries();

  LOG(INFO) << "Starting event loop";
  for (unsigned long int i_entry = 0; i_entry < n_entries; i_entry++){

    ch->GetEntry(i_entry);
    if (i_entry%10000 == 0) LOG(INFO) << "Reading entry " << i_entry;

    // Skip events without MCTracks. This is very rare, but it can happen if the MCEventBuilder is used to process simulated events.
    if (isMC and MCTracks->GetEntries() == 0){
      LOG(WARNING) << "Warning: Skipping event " << std::to_string(i_entry) << " because it has zero MCTracks.";
      continue;
    }

    // Initialize event
    scifiDet->InitEvent(event_header);
    mufilterDet->InitEvent(event_header);
    
    // Process pipeline
    bool accept_event = true;
    for (snd::analysis_core::baseProcess * proc : pipeline){
      // Run process
      proc->process();
      // If process is a cut, check if it passes
      snd::analysis_cuts::baseCut * cut = dynamic_cast<snd::analysis_cuts::baseCut*>(proc);
      if (cut){
	if (not cut->passCut()) accept_event = false;
      }
    }
    // If all cuts passed fill output TTree
    if (accept_event) outTree->Fill();
  }

  // Write output file and clean up
  outFile->Write();

  delete outTree;
  delete outFile;
  delete ch;
  
  LOG(INFO) << "sndEventFilter ends";
  return 0;
}
