#include <iostream>

#include "sndCutFlowHist.h"
#include "sndFilterProcessBase.h"
#include "sndBaseCut.h"

#include "TROOT.h"
#include "TDirectory.h"
#include "TGlobal.h"

#include "ShipMCTrack.h"

#include "FairLogger.h"

namespace snd {
  namespace analysis_processes {
    cutFlowHist::cutFlowHist(){

      processName = "Cut flow histograms";
      
      isMC = true;
      if (gROOT->GetListOfGlobals()->FindObject("rawConv")) isMC = false;

      // Get MCTracks
      if (isMC) {
	MCTracks = dynamic_cast<TClonesArray*>(gROOT->GetListOfGlobals()->FindObject("ShipMCTracks"));
	if (! MCTracks) {
	  LOG(FATAL) << "cutFlowHist ERROR: MCTracks not found";
	}
      }
      
	
      // Get the pipeline
      std::vector< snd::analysis_core::baseProcess * > * pipeline_ptr = static_cast<std::vector< snd::analysis_core::baseProcess * > *>(static_cast<TGlobal*>(gROOT->GetListOfGlobals()->FindObject("pipeline"))->GetAddress());
      if (!pipeline_ptr) LOG(FATAL) << "sndCutFlowHist could not get pointer to pipeline!";
      std::vector< snd::analysis_core::baseProcess * > & pipeline = *pipeline_ptr;
      
      // Store pointers to cut processes in a separate vector, for convenience
      cuts = new std::vector<snd::analysis_cuts::baseCut*>();
    
      for (snd::analysis_core::baseProcess * proc : pipeline){
	if (dynamic_cast<snd::analysis_cuts::baseCut*>(proc)){
	  cuts->push_back(dynamic_cast<snd::analysis_cuts::baseCut*>(proc));
	}
      }

      // Book histograms
      // Cut-by-cut
      // All cut variables
      cut_by_cut_var_histos = new std::vector<std::vector<TH1D*> >();
      for (int i_cut = -1; i_cut < static_cast<int>(cuts->size()); i_cut++){
	std::vector<TH1D*> this_cut_by_cut_var_histos = std::vector<TH1D*>();

	for (snd::analysis_cuts::baseCut * cut : *cuts) {
	  for(long unsigned int i_dim = 0; i_dim < cut->getNbins().size(); i_dim++){
	    this_cut_by_cut_var_histos.push_back(new TH1D(("cutFlow_"+std::to_string(i_cut)+"_"+cut->getShortName()+"_"+std::to_string(i_dim)).c_str(), 
							  cut->getShortName().c_str(), 
							  cut->getNbins()[i_dim], cut->getRangeStart()[i_dim], cut->getRangeEnd()[i_dim]));
	  }
	}
	cut_by_cut_var_histos->push_back(this_cut_by_cut_var_histos);
      }
    
      // Cut-by-cut MC truth variables
      if (isMC) {

	// Create directory for truth-based histograms in TFile
	TDirectory * top_dir = gDirectory;
	gDirectory->mkdir("TruthHistos");
	gDirectory->cd("TruthHistos");

	cutFlow_by_species = new std::vector<TH1D*>();
	
	cut_by_cut_truth_histos = new std::vector<std::vector<std::vector<TH1D*> > >();

	for (int i_species = 0; i_species < 6; i_species++){ // e, mu, tau0mu, tau1mu, NC, other
	  std::vector<std::vector<TH1D*> > this_species_histos = std::vector<std::vector<TH1D*> >();
	  std::string species_suffix;
	  switch (i_species) {
	  case 0:
	    species_suffix = "nueCC";
	    break;
	  case 1:
	    species_suffix = "numuCC";
	    break;
	  case 2:
	    species_suffix = "nutauCC0mu";
	    break;
	  case 3:
	    species_suffix = "nutauCC1mu";
	    break;
	  case 4:
	    species_suffix = "NC";
	    break;
	  case 5:
	    species_suffix = "Other"; // For PG sim
	    break;
	  default :
	    LOG(FATAL) << "MC truth histograms initialization error! Unknown species";
	  }

	  cutFlow_by_species->push_back(new TH1D((species_suffix+"_cutFlow").c_str(), "Cut flow;;Number of events passing cut", cuts->size()+1, 0, cuts->size()+1));
	  for (int i = 2; i <= dynamic_cast<TH1D*>(cutFlow_by_species->back())->GetNbinsX(); i++){
	    dynamic_cast<TH1D*>(cutFlow_by_species->back())->GetXaxis()->SetBinLabel(i, cuts->at(i-2)->getName().c_str());
	  }
	  
	  for (int i_cut = -1; i_cut < static_cast<int>(cuts->size()); i_cut++){
	  
	    std::vector<TH1D*> this_cut_by_cut_truth_histos = std::vector<TH1D*>();
	    this_cut_by_cut_truth_histos.push_back(new TH1D((species_suffix+"_"+std::to_string(i_cut)+"_Enu").c_str(), "Enu", 300, 0, 3000));
	    this_cut_by_cut_truth_histos.push_back(new TH1D((species_suffix+"_"+std::to_string(i_cut)+"_EEM").c_str(), "ELep", 300, 0, 3000));
	    this_cut_by_cut_truth_histos.push_back(new TH1D((species_suffix+"_"+std::to_string(i_cut)+"_EHad").c_str(), "EHad", 300, 0, 3000));
	    this_cut_by_cut_truth_histos.push_back(new TH1D((species_suffix+"_"+std::to_string(i_cut)+"_vtxX").c_str(), "vtxX", 200, -100, 0));
	    this_cut_by_cut_truth_histos.push_back(new TH1D((species_suffix+"_"+std::to_string(i_cut)+"_vtxY").c_str(), "vtxY", 200, 0, 100));
	    this_cut_by_cut_truth_histos.push_back(new TH1D((species_suffix+"_"+std::to_string(i_cut)+"_vtxZ").c_str(), "vtxZ", 200, 280, 380));
	  
	    this_species_histos.push_back(this_cut_by_cut_truth_histos);
	  }
	  cut_by_cut_truth_histos->push_back(this_species_histos);
	}
	top_dir->cd();
      }

      // N-1
      n_minus_1_var_histos = new std::vector<TH1D*>();
      for (snd::analysis_cuts::baseCut * cut : *cuts) {
	for(long unsigned int i_dim = 0; i_dim < cut->getNbins().size(); i_dim++){
	  n_minus_1_var_histos->push_back(new TH1D(("n_minus_1_"+cut->getShortName()+"_"+std::to_string(i_dim)).c_str(), 
						  cut->getShortName().c_str(), 
						  cut->getNbins()[i_dim], cut->getRangeStart()[i_dim], cut->getRangeEnd()[i_dim]));
	}
      }
    
      // Cut flow
      cutFlowHistogram = new TH1D("cutFlow", "Cut flow;;Number of events passing cut", cuts->size()+1, 0, cuts->size()+1);
      for (int i = 2; i <= cutFlowHistogram->GetNbinsX(); i++){
	cutFlowHistogram->GetXaxis()->SetBinLabel(i, cuts->at(i-2)->getName().c_str());
      }
    }
  
    void cutFlowHist::process(){

      // Fill 1st bin of cut flow for all the events
      cutFlowHistogram->Fill(0);

      // Fill cut flow histogram
      bool accept_event = true;
      int i_cut = 0;
      int n_cuts_passed = 0;
      for (snd::analysis_cuts::baseCut * cut : *cuts) {
	// Count number of cuts passed for this event. Useful for N-1 histograms below
	if (not cut->passCut()) accept_event = false;
	else n_cuts_passed += 1;
	// If all cuts up to the current cut have passed, fill cutFlow histogram:
	if (accept_event) cutFlowHistogram->Fill(i_cut + 1);
      
	i_cut++;
      }

      if (isMC){
	int this_species = getSpecies(MCTracks);
	// Fill cut flow histogram
	cutFlow_by_species->at(this_species)->Fill(0);
	int i_cut_species = 0;
	for (snd::analysis_cuts::baseCut * cut : *cuts) {
	  if (not cut->passCut()) break;
	  cutFlow_by_species->at(this_species)->Fill(i_cut_species + 1);
	  i_cut_species += 1;
	}
      }

      // Fill other histograms
      std::vector<TH1D*>::iterator hist_it;

      // Sequential
      for (int seq_cut = -1; seq_cut < static_cast<int>(cuts->size()); seq_cut++){
	if (seq_cut >= 0){
	  if (not cuts->at(seq_cut)->passCut()) break;
	}
	hist_it = cut_by_cut_var_histos->at(seq_cut+1).begin();
	for (snd::analysis_cuts::baseCut * cut : *cuts){
	  for (long unsigned int i_dim = 0; i_dim < cut->getPlotVar().size(); i_dim++){
	    (*hist_it)->Fill(cut->getPlotVar()[i_dim]);
	    hist_it++;
	  }
	}
      
	if (isMC){
	  int this_species = getSpecies(MCTracks);
	  
	  cut_by_cut_truth_histos->at(this_species)[seq_cut+1][0]->Fill(dynamic_cast<ShipMCTrack*>(MCTracks->At(0))->GetEnergy()); // Enu
	  if (this_species < 5) {
	    cut_by_cut_truth_histos->at(this_species)[seq_cut+1][1]->Fill(dynamic_cast<ShipMCTrack*>(MCTracks->At(1))->GetEnergy()); // ELep
	    cut_by_cut_truth_histos->at(this_species)[seq_cut+1][2]->Fill(dynamic_cast<ShipMCTrack*>(MCTracks->At(0))->GetEnergy()-((ShipMCTrack*) MCTracks->At(1))->GetEnergy()); // EHad
	  }
	  cut_by_cut_truth_histos->at(this_species)[seq_cut+1][3]->Fill(dynamic_cast<ShipMCTrack*>(MCTracks->At(0))->GetStartX()); // X
	  cut_by_cut_truth_histos->at(this_species)[seq_cut+1][4]->Fill(dynamic_cast<ShipMCTrack*>(MCTracks->At(0))->GetStartY()); // Y
	  cut_by_cut_truth_histos->at(this_species)[seq_cut+1][5]->Fill(dynamic_cast<ShipMCTrack*>(MCTracks->At(0))->GetStartZ()); // Z
	}
      }

      // N-1
      int current_cut = 0;
      hist_it = n_minus_1_var_histos->begin();
      for (snd::analysis_cuts::baseCut * cut : *cuts) {
	for (long unsigned int i_dim = 0; i_dim < cut->getPlotVar().size(); i_dim++){
	  if (((not cut->passCut()) and (n_cuts_passed == (static_cast<int>(cuts->size())-1)))
	      or (n_cuts_passed == static_cast<int>(cuts->size())) ) (*hist_it)->Fill(cut->getPlotVar()[i_dim]);
	  hist_it++;
	}
	current_cut++;
      }
    }
    
    unsigned int cutFlowHist::getSpecies(TClonesArray* tracks){
      int species = -1;
      if (tracks->GetEntries() < 2) {
	species = 5;
      } else {
	  
	int pdgIn = abs(dynamic_cast<ShipMCTrack*>(tracks->At(0))->GetPdgCode());
	int pdgOut = abs(dynamic_cast<ShipMCTrack*>(tracks->At(1))->GetPdgCode());
	
	// TEMP HACK to deal with MC event builder rare cases where the outgoing lepton is not in the MCTrack collection.
	// In neutral current events, the outgoing neutrino is forced to be in the collection. So, if the MCTrack at index 1 has invalid (== 0) PDG code, assume it's a charged current interaction.
	if (pdgOut == 0){
	  pdgOut = pdgIn - 1;
	  LOG(WARNING) << "Encountered 0 in MCTrack[1] PDG code. Assuming CC interaction. PDG IN " << std::to_string(pdgIn) << " PDG OUT " << std::to_string(pdgOut);
	}
	
	if (pdgIn == (pdgOut+1)){
	  //CC
	  if (pdgIn == 12) species = 0; // nueCC
	  if (pdgIn == 14) species = 1; // numuCC
	  if (pdgIn == 16) {
	    bool is1Mu = false;
	    for(int j_track = 2; j_track < tracks->GetEntries(); j_track++){
	      if (dynamic_cast<ShipMCTrack*>(tracks->At(j_track))->GetMotherId() == 1 and abs(((ShipMCTrack*)tracks->At(j_track))->GetPdgCode()) == 13){
		is1Mu = true;
		break;
	      }
	    }
	    if (is1Mu) species = 3; // nutauCC1mu
	    else species = 2; // nutauCC0mu
	  }
	} else if (pdgIn == pdgOut) {
	  //NC
	  species = 4;
	} else {
	  // Other
	  species = 5;
	}
      }
      return species;
    }
  }
}
