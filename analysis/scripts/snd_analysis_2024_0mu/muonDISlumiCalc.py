gen_pp_collisions = 200e6 # Muon flux equivalent to 200M pp collisions
pp_inclusive_xsec = 78e-3 # barn
n_mult = 2*100 # Each muon ray is simulated 10 times for proton interactions and 10 times for neutron interactions
L_muon = n_mult*gen_pp_collisions/pp_inclusive_xsec # barn-1
m_nuc = 1.67e-24 # grams

import ROOT
weights = []
cbmsim = ROOT.TChain("cbmsim")
for i_run in [201, 501]:
    cbmsim.Add("/eos/experiment/sndlhc/users/dancc/MuonDIS/ecut1.0_z-7_2.5m_Ioni_latelateFLUKA/muonDis_{0}/1/sndLHC.muonDIS-TGeant4-muonDis_{0}_digCPP.root".format(i_run))

g = {}
f_xsec = ROOT.TFile("/eos/experiment/sndlhc/MonteCarlo/Pythia6/MuonDIS/muDIScrossSec.root")

g[13] = f_xsec.Get("g_13")
g[-13] = f_xsec.Get("g_-13")

for event in cbmsim:
    w = event.MCTrack[0].GetWeight() # FLUKA, probability
    w *= event.MCTrack[2].GetWeight() # rho x length [g/cm3 x cm] [g/cm2]
    w /= m_nuc # [cm=2]
    # ASSUME pythia xsec is in mbarn
    w *= g[event.MCTrack[0].GetPdgCode()].Eval(event.MCTrack[0].GetEnergy())*1e-27 #[cm2]
#    w *= g[event.MCTrack[0].GetPdgCode()].Eval(event.MCTrack[0].GetEnergy())*0.6e-3 # [UNITS?]
    weights.append(w)
import numpy as np

DIS_interaction_probability = np.mean(weights)

print("DIS interaction probability: {:2.2e}".format(DIS_interaction_probability))

TARGET_LUMI = 68.5e15

print("DIS equivalent luminosity: {:2.2e} fb-1".format(L_muon/DIS_interaction_probability))

print("68.3% upper limit on DIS events in nue selection for {:2.2e} fb-1: {:.2e}".format(TARGET_LUMI/1e15, TARGET_LUMI/(L_muon/DIS_interaction_probability)*1.15))
print("90%   upper limit on DIS events in nue selection for {:2.2e} fb-1: {:.2e}".format(TARGET_LUMI/1e15, TARGET_LUMI/(L_muon/DIS_interaction_probability)*2.3))

#print(70e15/(2*20*200e6/80e-3)*np.mean(weights)*1.15)
