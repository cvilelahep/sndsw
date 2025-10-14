# Search for 0mu events

Analysis first presented at Moriond QCD Moriond 2024.

## Steps to run the analysis
 - Run neutrinoFilterGoldernSample with option nueFilter (5)
   - Samples: all 2022 and 2023 data, neutrino MC, all neutral hadron files labeled _tgtarea (in marssnd experiment area), all muDIS files (large sample in cvilela experiment area).
 - Run calculateHadronRates_nue.py, with "make_background_tree = True"
   - This will list tables of events passing each cut, and it will produce a TTree with neutral hadron events with weights that reproduce the spectrum of the highest energy hadron in muDIS interactions that penetrates the target.
 - Run nueHitDensityHistograms.py
   - This will calculate the hit density variables and select only event over a minimum threshold of 20 in at least two stations and 2000 in the maximum station.
 - Run rs_nue_significance.C
   - This script normalizes the neutral hadron distribution to the predicted event rate in the region with hit densities between [2000, 5000] and calculates the expected significance over a range of signal region lower thresholds.
   - It also calculates the observed significance, but note that the number of observed events is *HARD CODED*.

## Auxiliary steps
 - muonDISlumiCalc.py
   - This script calculates the equivalent lumi of the large muDIS sample production, and the upper limit on the number of background events due to muon DIS in the tunnel walls.
 - makePrettyPlots.py
   - Produces plots for talks/publications
 
   