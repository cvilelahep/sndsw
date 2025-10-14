import ROOT
import xml.etree.ElementTree as ET
import numpy as np
import tarfile
import shutil
import os
from glob import glob
from tqdm import tqdm

from datetime import datetime, timedelta
dt_format = "%Y-%m-%d %H:%M:%S"

import shipunit

from collections import defaultdict

def fstr(template):
    return eval(f'f"""{template}"""')

xml_run_list = "numu_22_23_v2_1762801091.542757.xml"

data_stage1_path = "/eos/experiment/sndlhc/users/cvilela/numu_22_23_data_reprocess_TSpectrumScifiFilter/run_{run_number:06d}/snd_event_filter_stage1_{run_number:06d}.root"
data_stage2_path = "/eos/experiment/sndlhc/users/cvilela/numu_22_23_data_reprocess_TSpectrumScifiFilter/run_{run_number:06d}/snd_event_filter_stage2_{run_number:06d}.root"

N_MC = 50
lumi_per_MC_file = 100.
MC_stage1_path = "/afs/cern.ch/work/c/cvilela/public/SND_Oct25_neutrinoSelDev_Oct7/reproMC_evtBuilder_target/filtered_TSpectrumSciFiFilter_{i_MC}.root"
MC_stage2_path = "/afs/cern.ch/work/c/cvilela/public/SND_Oct25_neutrinoSelDev_Oct7/reproMC_evtBuilder_target/filtered_TSpectrumSciFiFilter_{i_MC}_stage2.root"

ATLAS_lumi_path = {2022: "/eos/project/a/atlas-datasummary/public/lumifiles/2022-OflLumi-Run3-003/lumi/",
                   2023: "/eos/project/a/atlas-datasummary/public/lumifiles/2023-OflLumi-Run3-004/lumi/"}

converted_data_path = {2022: "/eos/experiment/sndlhc/convertedData/physics/2022/",
                       2023: "/eos/experiment/sndlhc/convertedData/physics/2023/"}

snd_ATLAS_lumi_path = "/eos/experiment/sndlhc/atlas_lumi/"

output_plots_path = "/eos/experiment/sndlhc/users/cvilela/numu_22_23_plots/"

xml_tree = ET.parse(xml_run_list)
xml_root = xml_tree.getroot()

all_runs = xml_root.find('runs').findall('run')

candidates = []

cut_flow_run_by_run = {}
cut_flow_data_total = defaultdict(int)

run_fill = {}
run_year = {}
run_start = {}

print("Getting data cutflows")
for run in tqdm(all_runs):
    run_number = int(run.find('run_number').text)
    start = run.find('start').text
    start_datetime = datetime.strptime(start, dt_format)
    fill_number = int(run.find('fill_number').text)

    run_fill[run_number] = fill_number
    run_year[run_number] = start_datetime.year
    run_start[run_number] = start_datetime
    
    events_passed_cuts = []

    try:
        f_stage1 = ROOT.TFile(fstr(data_stage1_path))
        for i in range(1, f_stage1.cutFlow.GetNbinsX()+1):
            cut_flow_data_total[f_stage1.cutFlow.GetXaxis().GetBinLabel(i)] += f_stage1.cutFlow.GetBinContent(i)
            events_passed_cuts.append(f_stage1.cutFlow.GetBinContent(i))
        f_stage1.Close()
    except OSError:
        print(f"WARNING! No stage1 file found for run {run_number}. Skipping run!!!")
        continue
    
    try:
        f_stage2 = ROOT.TFile(fstr(data_stage2_path))

        for i in range(2, f_stage2.cutFlow.GetNbinsX()+1):
            cut_flow_data_total[f_stage2.cutFlow.GetXaxis().GetBinLabel(i)] += f_stage2.cutFlow.GetBinContent(i)
            events_passed_cuts.append(f_stage2.cutFlow.GetBinContent(i))
        
        for event in f_stage2.rawConv:

            ev_time = event.EventHeader.GetEventTime()*shipunit.snd_TDC2ns/1e9
            start_plus_time = (start_datetime + timedelta(seconds = ev_time)).strftime(dt_format)
            
            candidates.append({"run_id": event.EventHeader.GetRunId(),
                               "event_number": event.EventHeader.GetEventNumber(),
                               "event_time": start_plus_time,
                               "fill_number": fill_number})
        f_stage2.Close()
    except OSError:
        print(f"WARNING! No stage2 file found for run {run_number}. Continuing!!!")
    
    cut_flow_run_by_run[run_number] = events_passed_cuts

################################################################################
# Candidate list
################################################################################
print("Candidate list")
with open("candidate_table.tex", "w") as f_tab:
    f_tab.write("Run number & Event number & Event timestamp & Fill number \\\\ \\hline \n")
    for cand in candidates:
        f_tab.write(f"{cand['run_id']} & {cand['event_number']} & {cand['event_time']} & {cand['fill_number']} \\\\ \n")
        print(f"{cand['run_id']} {cand['event_number']} {cand['event_time']} {cand['fill_number']}")
print()


################################################################################
# Get lumi per run
################################################################################
print("Getting Lumi from ATLAS source dir")

runs = cut_flow_run_by_run.keys()
fill_lumi = {}
for r in tqdm(runs):
    f_name_lumi = ATLAS_lumi_path[run_year[r]]+"/"+str(int(run_fill[r]))+".tgz"

    with tarfile.open(f_name_lumi, 'r') as tar:
        tar.extract(str(int(run_fill[r]))+'/'+str(int(run_fill[r]))+'_summary_ATLAS.txt')
    with open(str(int(run_fill[r]))+'/'+str(int(run_fill[r]))+'_summary_ATLAS.txt', 'r') as f_lumi:
        for line in f_lumi:
            fill_lumi[r] = float(line.split()[-1])
            break
    shutil.rmtree(str(int(run_fill[r])))

print("Integrating Lumi for each run (from SND@LHC copy of ATLAS data")

run_int_lumi = {}
run_duration = {}

for r in tqdm(runs):

    # Fine-tune by finding first event in stable beams and taking that timestamp as the start
    this_start = run_start[r]

    last_root_file_name = glob(converted_data_path[run_year[r]]+f"/run_{r:06d}/*.root")[-1]

    f_last_root = ROOT.TFile(last_root_file_name)
    t_last_root = f_last_root["rawConv"]
    n_entries = t_last_root.GetEntries()
    t_last_root.GetEntry(n_entries)
    t_last_root.GetEntry(n_entries-1)
    
    run_duration[r] = timedelta(seconds = t_last_root.EventHeader.GetEventTime()*shipunit.snd_TDC2ns/1e9)

    f_last_root.Close()
    
    this_end = this_start+run_duration[r]
    f_atlas_lumi = ROOT.TFile(snd_ATLAS_lumi_path+f"fill_{run_fill[r]:06d}.root")
    prev_lumi = -1
    prev_t = -1

    run_int_lumi[r] = 0.

    for reading in f_atlas_lumi["atlas_lumi"]:
        t = datetime.utcfromtimestamp(reading.unix_timestamp)
        if t < this_start:
            continue
        if t > this_end:
            continue
        if prev_lumi >= 0:
            run_int_lumi[r] += (reading.var+prev_lumi)/2.*(t-prev_t).seconds
            
        prev_t = t
        prev_lumi = reading.var
    f_atlas_lumi.Close()

################################################################################
# Run list table
################################################################################
print("Run list")
with open("run_list_table.tex", "w") as f_tab:
    f_tab.write("Run number & Start time & Duration (hours) & Integrated luminosity (fb-1) & Fill number & Fill luminosity (fb-1) \\\\ \\hline \n")
    for run in runs:
        f_tab.write(f"{run} & {run_start[run].strftime(dt_format)} & {run_duration[run].seconds/60./60.:.2f} & {run_int_lumi[run]/1e9:.2e} & {run_fill[run]} & {fill_lumi[run]/1e9:.2e} \\\\ \n")
        print(f"{run} {run_start[run].strftime(dt_format)} {run_duration[run].seconds/60./60.:.2f} {run_int_lumi[run]/1e9:.2e} {run_fill[run]} {fill_lumi[run]/1e9:.2e}")
    tot_lumi = sum([l for k, l in run_int_lumi.items()])
    tot_duration = sum([d.seconds for k, d in run_duration.items()])
    f_tab.write("\hline\n")
    f_tab.write(f"TOTAL & & {tot_duration/60./60.:.2f} & {tot_lumi/1e9:.2e} &  &  \\\\ \n")
    print(f"Total {tot_duration/60./60.:.2f} {tot_lumi/1e9:.2e}")
print()

################################################################################
# Cut stability
################################################################################
N_max = 100
N_hists = len(runs)//N_max + 1
runs_split = [list(runs)[i*N_max:(i+1)*N_max] for i in range(N_hists)]

stability_hists = []

rate_by_cut = defaultdict(int)
tot_lumi = sum([l for k, l in run_int_lumi.items()])

for i_cut, cut_name in enumerate(cut_flow_data_total.keys()):
    these_hists = []
    for i_split in range(N_hists):
        these_hists.append(ROOT.TH1D(f"stabilitity_cut_{i_cut-1:d}_{i_split}", f"{cut_name};Run number;Events/ub-1", N_max, 0, N_max))
        for i_run, run in enumerate(runs_split[i_split]):
            these_hists[-1].GetXaxis().SetBinLabel(i_run+1, str(run))
    stability_hists.append(these_hists)
for i_run, (run, cut_flow) in enumerate(cut_flow_run_by_run.items()):
    i_split = i_run // N_max
    for i_cut, n in enumerate(cut_flow):
        rate_by_cut[i_cut] += n
        if run_int_lumi[run] > 0:
            stability_hists[i_cut][i_split].SetBinContent(i_run%N_max+1, n/run_int_lumi[run])
            stability_hists[i_cut][i_split].SetBinError(i_run%N_max+1, n**0.5/run_int_lumi[run])

os.makedirs(output_plots_path+"/cut_stability", exist_ok=True)

ROOT.gROOT.SetBatch()
ROOT.gStyle.SetOptStat(0)
c_stability = ROOT.TCanvas("cutStability", "cut stability", 2000, 2000)
c_stability.Divide(1, N_hists)
for i_cut, h_list in enumerate(stability_hists):
    avg_lines = []
    for i_h, h in enumerate(h_list):
        c_stability.cd(i_h+1)
        h.SetMaximum(rate_by_cut[i_cut]/tot_lumi*10)
        h.SetMinimum(-rate_by_cut[i_cut]/tot_lumi)
        avg_lines.append(ROOT.TLine(0, rate_by_cut[i_cut]/tot_lumi, N_max, rate_by_cut[i_cut]/tot_lumi))
        avg_lines[-1].SetLineStyle(2)
        avg_lines[-1].SetLineColor(ROOT.kBlack)
        h.Draw("EP")
        avg_lines[-1].Draw("SAME")
        h.Draw("SAMEP")
        
    c_stability.SaveAs(f"{output_plots_path}/cut_stability/cut_stability_{i_cut}.pdf")
    c_stability.SaveAs(f"{output_plots_path}/cut_stability/cut_stability_{i_cut}.png")


################################################################################
# MC cut flow
################################################################################
N_MC_FOR_LUMI = 0
nu_samples = ["numuCC", "NC", "nueCC", "nutauCC0mu", "nutauCC1mu"]
cut_flow_MC = {}

for sample in nu_samples:
    cut_flow_MC[sample] = defaultdict(int)

print("Loading MC")
for i_MC in tqdm(range(0, N_MC)):
    try:
        f_stage1 = ROOT.TFile(fstr(MC_stage1_path))
        for sample in nu_samples:
            this_cut_flow = f_stage1.Get(f"TruthHistos/{sample}_cutFlow")
            for i in range(1, this_cut_flow.GetNbinsX()+1):
                print(f"{i_MC} {sample} {i} {this_cut_flow.GetBinContent(i)}")
                cut_flow_MC[sample][this_cut_flow.GetXaxis().GetBinLabel(i)] += this_cut_flow.GetBinContent(i)
        f_stage1.Close()
    except OSError:
        print(f"WARNING! No stage1 file found for MC file {i_MC}. Skipping run!!!")
        continue
    try:
        f_stage2 = ROOT.TFile(fstr(MC_stage2_path))
        for sample in nu_samples:
            this_cut_flow = f_stage2.Get(f"TruthHistos/{sample}_cutFlow")
            for i in range(2, this_cut_flow.GetNbinsX()+1):
                cut_flow_MC[sample][this_cut_flow.GetXaxis().GetBinLabel(i)] += this_cut_flow.GetBinContent(i)
        f_stage2.Close()
    except OSError:
        print(f"WARNING! No stage2 file found for MC file {i_MC}.")
    N_MC_FOR_LUMI += 1
    
################################################################################
# Cut flow table and histograms
################################################################################

LUMI_NORM = 1./(N_MC_FOR_LUMI*lumi_per_MC_file)*tot_lumi/1e9

print("Cut flow table")
with open("cutflow_table.tex", "w") as f_tab:
    header_string = ""
    subheader_string = ""
    subheader_string += "Cut"
    for sample in ["Data"] + nu_samples:
        header_string += " & \multicolumn{2}{|c|}{"+sample+"}"
        subheader_string += " & Events & Efficiency"
    header_string += "\\\\ \n"
    subheader_string += "\\\\ \\hline \n"

    f_tab.write(header_string)
    f_tab.write(subheader_string)

    prev_cut_events = {}
    for i_cut, cut_name in enumerate(cut_flow_data_total.keys()):
        row = ""
        row += f"{cut_name: <80}"
        row += f" & {cut_flow_data_total[cut_name]: <15}"
        if i_cut:
            row += f" & {cut_flow_data_total[cut_name]/prev_cut_events['Data']:6.2e}"
        else :
            row += " & "
        prev_cut_events["Data"] = cut_flow_data_total[cut_name]

        # Hack for US QDC cut
        cut_name = cut_name.replace("600", "700")
        
        for sample in nu_samples:
            row += f" & {cut_flow_MC[sample][cut_name]*LUMI_NORM:6.2f}"
            if i_cut and prev_cut_events[sample] > 0:
                row += f" & {cut_flow_MC[sample][cut_name]/prev_cut_events[sample]:6.2e}"
            prev_cut_events[sample] = cut_flow_MC[sample][cut_name]
        row += "\\\\ \n"

        f_tab.write(row)
    
################################################################################
# MC efficiency plots
################################################################################
# Flavour
# Enu
# Vtx x, y, z
# Lepton angle
# Hadron energy

#filtered_MC_file_path = ""
#MC_index_list = [i for i in range(50) if not i == 3]


################################################################################
# Data / MC plots
################################################################################
# Wall start
# theta_mu
# Ehad (Check with Filippo if code available?)
# N scifi hits

# Here it would be nice to include different generators
# Also, muon background should be here included

################################################################################
# Flux vs cross section constraint
################################################################################


