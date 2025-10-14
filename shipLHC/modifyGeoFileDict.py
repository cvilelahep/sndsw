#!/usr/bin/env python
import ROOT,os
from rootpyPickler import Pickler
from rootpyPickler import Unpickler
import shipunit as u
import saveBasicParameters
from XRootD import client

theClient = client.FileSystem('root://eospublic.cern.ch')
commonPath = "/eos/experiment/sndlhc/convertedData/physics/"

supportedGeoFiles = {}
supported_years = [2022, 2023, 2024, 2025]
for year in supported_years:
   supportedGeoFiles["geofile_sndlhc_TI18_V0_"+str(year)+".root"] = commonPath+str(year)+"/"

def modifyDicts(year=2024):
   if year not in supported_years:
      print("Geometry for the required year "+str(year)+"is not supported.\n\
             The list of supported years is:", supported_years)
      return
   for geoFileName in supportedGeoFiles:
         if geoFileName.find(str(year)) > 0:
            # override locally existing file with the same name
            status = theClient.copy(supportedGeoFiles[geoFileName]+geoFileName,
                                    os.getcwd()+'/'+geoFileName, force=True)
            if not status[0].ok: 
               print('Bad status of the local copy process', status)
         else: continue
         fg  = ROOT.TFile(geoFileName)
         pkl = Unpickler(fg)
         sGeo = pkl.load('ShipGeo')
         fg.Close()

         # MuFilter spatial alignment: re-done when new MuFilter detectors are added
         # or detectors are moved.
         mufi_spatial_aligment_consts = {
           2022: [ 0.11,-0.04, 0.00, 0.10, 0.26, 0.24, 0.31, 0.34, 0.43, 1.13, 0.53, 1.31, 0.61, 1.35, 1.39],
           2023: [ 0.15, 0.03, 0.00,-0.01, 0.09,-0.01, 0.06, 0.06, 0.49, 0.86, 0.20, 0.98, 0.24, 1.01, 1.11],
           2024: [-0.06, 0.04, 0.65,-0.15,-0.12,-0.26,-0.29,-0.36, 0.00, 0.35,-0.37, 0.38,-0.41, 0.30, 0.35],
           2025: [-0.04, 0.05, 0.66,-0.16,-0.15,-0.31,-0.34,-0.45,-0.10, 0.39,-0.48, 0.43,-0.91,-0.36,-2.02]
         }
         mufi_spatial_aligment_keys = ['Veto1ShiftY','Veto2ShiftY','Veto3ShiftX',
                                       'US1ShiftY','US2ShiftY','US3ShiftY','US4ShiftY','US5ShiftY',
                                       'DS1ShiftY','DS1ShiftX','DS2ShiftY','DS2ShiftX',
                                       'DS3ShiftY','DS3ShiftX','DS4ShiftX']
         for i, key in enumerate(mufi_spatial_aligment_keys):
           if year < 2024 and key == 'Veto3ShiftX': continue
           setattr(sGeo.MuFilter,key,mufi_spatial_aligment_consts[year][i]*u.cm)
           sGeo.MuFilter[key] = mufi_spatial_aligment_consts[year][i]*u.cm

         # DS part
         constants = {}
         constants['t_0'] =  [0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0]
         constants['t_4361'] =  [-5.61,-5.63,-5.90,-5.39,-5.40,-5.58,-5.99,-6.08,-6.27,-6.43,-5.94,-6.20,-5.45,-5.52,-5.75,-5.93,-5.40,-5.56,-5.46,-5.74]
         constants['t_5117'] =  [-6.56,-6.53,-6.75,-6.27,-6.34,-6.46,-7.50,-7.60,-7.79,-7.94,-7.51,-7.72,-8.09,-8.15,-8.38,-8.55,-8.04,-8.14,-8.02,-8.24]
         constants['t_5478'] =  [-5.28,-5.38,-5.71,-5.76,-5.28,-5.46,-4.22,-4.38,-4.55,-4.70,-4.27,-4.46,-5.31,-5.43,-5.71,-5.88,-5.34,-5.45,-5.38,-5.57]
         constants['t_6208'] =  [-6.00,-5.92,-6.33,-6.22,-5.94,-6.15,-5.53,-5.49,-5.81,-5.86,-5.53,-5.77,-7.74,-7.71,-8.01,-8.04,-7.68,-7.82,-7.65,-7.87]
         constants['t_6443'] =  [-6.56,-6.70,-6.78,-6.88,-6.40,-6.59,-5.97,-6.08,-6.15,-6.29,-5.93,-6.14,-8.10,-8.20,-8.41,-8.56,-8.11,-8.21,-8.20,-8.39]
         constants['t_6677'] =  [-6.72,-6.80,-6.87,-6.87,-6.49,-6.67,-6.03,-6.16,-6.35,-6.49,-6.08,-6.29,-8.23,-8.36,-8.54,-8.69,-8.22,-8.38,-8.27,-8.47]
         # 2024 and later on
         constants['t_7649'] =  [-6.43,-6.56,-6.76,-6.83,-6.46,-6.59,-5.97,-6.04,-6.24,-6.42,-6.04,-6.24,-8.18,-8.26,-8.44,-8.61,-8.18,-8.26,-8.15,-8.31]
         constants['t_8318'] =  [-6.50,-6.66,-6.69,-6.75,-6.36,-6.53,-5.85,-5.92,-6.19,-6.34,-5.96,-6.18,-8.09,-8.23,-8.36,-8.56,-8.10,-8.16,-8.10,-8.31]
         constants['t_8583'] =  [-6.61,-6.68,-6.79,-6.81,-6.45,-6.62,-6.01,-6.17,-6.23,-6.35,-6.01,-6.24,-8.16,-8.29,-8.44,-8.62,-8.21,-8.35,-8.30,-8.48]
         constants['t_8942'] =  [-6.44,-6.52,-6.72,-6.81,-6.39,-6.53,-6.04,-6.06,-6.18,-6.34,-5.98,-6.17,-8.15,-8.13,-8.39,-8.57,-8.13,-8.19,-8.16,-8.29]
         constants['t_9156'] =  [-6.47,-6.50,-6.85,-6.92,-6.47,-6.58,-6.07,-6.20,-6.33,-6.48,-6.07,-6.28,-8.19,-8.30,-8.53,-8.67,-8.20,-8.32,-8.21,-8.37]
         constants['t_9286'] =  [-6.41,-6.48,-6.84,-6.89,-6.47,-6.62,-5.96,-6.05,-6.35,-6.47,-6.08,-6.28,-8.13,-8.20,-8.52,-8.70,-8.19,-8.31,-8.15,-8.33]
         constants['t_9379'] =  [-6.42,-6.54,-6.83,-6.88,-6.41,-6.60,-5.93,-6.00,-6.37,-6.49,-6.05,-6.30,-8.11,-8.20,-8.51,-8.63,-8.14,-8.25,-8.11,-8.31]
         constants['t_9462'] =  [-6.47,-6.56,-6.77,-6.86,-6.44,-6.58,-6.01,-6.04,-6.30,-6.52,-6.07,-6.29,-8.18,-8.25,-8.46,-8.65,-8.17,-8.26,-8.13,-8.24]
         constants['t_9613'] =  [-6.50,-6.55,-6.78,-6.88,-6.47,-6.63,-5.98,-6.01,-6.24,-6.38,-6.03,-6.19,-8.21,-8.25,-8.43,-8.67,-8.18,-8.25,-8.13,-8.29]
         constants['t_9692'] =  [-6.58,-6.67,-6.76,-6.95,-6.47,-6.53,-6.07,-6.20,-6.22,-6.39,-6.01,-6.18,-8.23,-8.26,-8.43,-8.65,-8.19,-8.27,-8.20,-8.35]
         constants['t_9882'] =  [-6.22,-6.13,-6.37,-6.25,-6.02,-6.18,-5.65,-5.58,-5.76,-5.79,-5.53,-5.75,-7.81,-7.69,-8.02,-8.07,-7.74,-7.85,-7.81,-7.98]
         constants['t_10012']=  [-6.70,-6.84,-6.73,-6.86,-6.50,-6.64,-6.04,-6.19,-6.21,-6.41,-6.05,-6.24,-8.23,-8.38,-8.40,-8.64,-8.22,-8.38,-8.31,-8.52]
         # 2025
         constants['t_10423']=  [-6.61,-6.69,-6.68,-6.77,-6.44,-6.58,-5.98,-6.06,-6.11,-6.29,-5.98,-6.16,-8.16,-8.17,-8.24,-8.43,-8.13,-8.22,-8.26,-8.41]
         constants['t_11158']=  [-6.60,-6.68,-6.79,-6.86,-6.51,-6.60,-6.08,-6.13,-6.20,-6.32,-6.04,-6.20,-8.19,-8.26,-8.34,-8.49,-8.19,-8.29,-8.31,-8.44]
         constants['t_11576']=  [-6.41,-6.48,-6.72,-6.78,-6.43,-6.53,-5.94,-6.06,-6.19,-6.36,-6.00,-6.17,-8.17,-8.20,-8.30,-8.49,-8.08,-8.16,-8.11,-8.21]
         constants['t_11676']=  [-6.42,-6.47,-6.74,-6.79,-6.43,-6.53,-5.95,-6.06,-6.21,-6.35,-6.01,-6.17,-8.16,-8.22,-8.32,-8.50,-8.09,-8.17,-8.13,-8.22]
         constants['t_11795']=  [-6.43,-6.45,-6.71,-6.79,-6.43,-6.52,-5.99,-6.09,-6.15,-6.32,-5.98,-6.13,-8.17,-8.25,-8.29,-8.48,-8.08,-8.14,-8.16,-8.24]
         ds_time_aligment_consts = {
           2022: {"t_0":0.000, "t_4361":0.082, "t_5117":0.085},
           2023: {"t_0":0.000, "t_5478":0.082, "t_6208":0.086, "t_6443":0.082, "t_6677":0.084},
           2024: {"t_0":0.000, "t_7649":0.081, "t_8318":0.082, "t_8583":0.081, "t_8942":0.080, 
                  "t_9156":0.083, "t_9286":0.082, "t_9379":0.083, "t_9462":0.083, "t_9613":0.082,
                  "t_9692":0.078, "t_9882":0.084, "t_10012":0.085},
           2025: {"t_0": 0.000, "t_10423":0.083, "t_11158":0.082, "t_11576":0.079, "t_11676":0.080,
                  "t_11795": 0.079}
         }
         slopes_dict = ds_time_aligment_consts[year]
         #time delay corrections first order, only for DS at the moment
         for p in slopes_dict.keys():
            setattr(sGeo.MuFilter,'DSTcorslope'+p,slopes_dict[p])
            sGeo.MuFilter['DSTcorslope'+p] = slopes_dict[p]
            for i in range(len(constants[p])): 
               setattr(sGeo.MuFilter,'DSTcorC'+str(i)+p,constants[p][i])
               sGeo.MuFilter['DSTcorC'+str(i)+p] = constants[p][i]

       # Scifi part
         constants={}
         constants['t_0']=[   0.000*u.ns,  0.000*u.ns,  0.000*u.ns, 0.000*u.ns, 0.000*u.ns,  0.000*u.ns, 0.000*u.ns,
  0.000*u.ns,  0.000*u.ns,   0.000*u.ns, 0.000*u.ns, 0.000*u.ns,  0.000*u.ns, 0.000*u.ns,
  0.000*u.ns,  0.000*u.ns,  0.000*u.ns,  0.000*u.ns,  0.000*u.ns,  0.000*u.ns,  0.000*u.ns,
   0.000*u.ns,  0.000*u.ns,  0.000*u.ns,  0.000*u.ns, 0.000*u.ns,  0.000*u.ns, 0.0000*u.ns,
   0.000*u.ns,  0.000*u.ns,  0.000*u.ns, 0.000*u.ns, 0.000*u.ns,  0.000*u.ns,  0.000*u.ns ]
         constants['t_4361']=[   0.000*u.ns,  0.000*u.ns,  -0.222*u.ns, -0.509*u.ns, -0.517*u.ns,  -1.156*u.ns, -0.771*u.ns,
  -0.287*u.ns,  0.000*u.ns,   0.250*u.ns, -0.854*u.ns, -1.455*u.ns,  -0.812*u.ns, -1.307*u.ns,
  -0.861*u.ns,  0.000*u.ns,  -0.307*u.ns,  0.289*u.ns,  0.069*u.ns,  -0.895*u.ns,  0.731*u.ns,
   0.164*u.ns,  0.000*u.ns,  -1.451*u.ns,  0.196*u.ns, -2.025*u.ns,  -1.049*u.ns, -0.938*u.ns,
   0.337*u.ns,  0.000*u.ns,  -1.157*u.ns, -1.060*u.ns, -0.627*u.ns,  -2.405*u.ns,  0.071*u.ns ]
         constants['t_5117']=[   0.000*u.ns,  0.000*u.ns,  -0.325*u.ns, -0.497*u.ns,  0.228*u.ns,  -0.368*u.ns,  0.076*u.ns,
  -0.906*u.ns,  0.000*u.ns,   0.259*u.ns, -0.775*u.ns, -0.370*u.ns,   0.243*u.ns, -0.284*u.ns,
  -0.995*u.ns,  0.000*u.ns,  -0.133*u.ns,  0.473*u.ns,  0.753*u.ns,  -0.433*u.ns,  1.106*u.ns,
  -0.371*u.ns,  0.000*u.ns,  -1.331*u.ns,  0.319*u.ns, -1.979*u.ns,  -1.120*u.ns, -0.981*u.ns,
  -1.276*u.ns,  0.000*u.ns,  -0.755*u.ns, -0.630*u.ns,  0.886*u.ns,  -0.873*u.ns,  1.639*u.ns ]
         constants['t_5478']=[   0.000*u.ns,  0.000*u.ns,  -0.309*u.ns,  -0.591*u.ns,  -0.502*u.ns,  -1.164*u.ns,  -0.753*u.ns, 
  -0.369*u.ns,  0.000*u.ns,  0.239*u.ns,  -0.815*u.ns,   -1.138*u.ns,  -0.578*u.ns,  -1.054*u.ns, 
  -1.011*u.ns,  0.000*u.ns,  -0.270*u.ns,  0.447*u.ns,   0.760*u.ns,  -0.512*u.ns,  0.566*u.ns, 
   0.024*u.ns,  0.000*u.ns,  -1.348*u.ns,  0.283*u.ns,   -1.495*u.ns,  -0.559*u.ns,  -0.545*u.ns, 
   0.195*u.ns,  0.000*u.ns,  -1.131*u.ns,  -0.937*u.ns,   -0.391*u.ns,  -2.102*u.ns,  0.365*u.ns ]
         constants['t_6208']=[   0.000*u.ns,  0.000*u.ns,  -0.309*u.ns,  -0.546*u.ns,   0.169*u.ns,  -0.454*u.ns,  -0.116*u.ns, 
  -0.920*u.ns,  0.000*u.ns,  0.238*u.ns,  -0.799*u.ns,   -0.430*u.ns,  0.186*u.ns,  -0.324*u.ns, 
  -1.042*u.ns,  0.000*u.ns,  -0.140*u.ns,  0.515*u.ns,   0.728*u.ns,  -0.572*u.ns,  0.673*u.ns, 
  -0.432*u.ns,  0.000*u.ns,  -1.366*u.ns,  0.326*u.ns,   -2.076*u.ns,  -1.152*u.ns,  -1.075*u.ns, 
  -1.067*u.ns,  0.000*u.ns,  -1.021*u.ns,  -0.873*u.ns,   0.623*u.ns,  -1.094*u.ns,  1.365*u.ns ]
         constants['t_6443']=[     0.000*u.ns,  0.000*u.ns,  -0.284*u.ns,  -0.469*u.ns,   0.274*u.ns,  -0.356*u.ns,  0.078*u.ns,
  -0.856*u.ns,  0.000*u.ns,  0.250*u.ns,  -0.754*u.ns,   -0.393*u.ns,  0.219*u.ns,  -0.828*u.ns,
  -0.939*u.ns,  0.000*u.ns,  -0.137*u.ns,  0.519*u.ns,   0.725*u.ns,  -0.480*u.ns,  1.024*u.ns,
  -0.311*u.ns,  0.000*u.ns,  -1.334*u.ns,  0.314*u.ns,   -2.028*u.ns,  -1.130*u.ns,  -1.037*u.ns,
  -0.979*u.ns,  0.000*u.ns,  -0.983*u.ns,  -0.862*u.ns,   0.627*u.ns,  -1.059*u.ns,  1.370*u.ns]
         constants['t_6677']=[   0.000*u.ns,  0.000*u.ns,  -0.340*u.ns,  -0.511*u.ns,   0.214*u.ns,  -0.387*u.ns,  -0.070*u.ns,
  -0.893*u.ns,  0.000*u.ns,  0.234*u.ns,  -0.762*u.ns,   -0.409*u.ns,  0.230*u.ns,  -0.291*u.ns, 
  -1.009*u.ns,  0.000*u.ns,  -0.140*u.ns,  0.546*u.ns,   0.762*u.ns,  -0.425*u.ns,  1.033*u.ns,
  -0.373*u.ns,  0.000*u.ns,  -1.356*u.ns,  0.322*u.ns,   -2.043*u.ns,  -1.122*u.ns,  -0.978*u.ns, 
  -1.055*u.ns,  0.000*u.ns,  -0.969*u.ns,  -0.823*u.ns,   0.644*u.ns,  -1.079*u.ns,  1.423*u.ns ]
#2024  
         constants['t_7649']=[     0.000*u.ns,  0.000*u.ns,  -0.299*u.ns,  -0.510*u.ns,   0.217*u.ns,  -0.401*u.ns,  0.016*u.ns,
  -1.194*u.ns,  0.000*u.ns,  0.547*u.ns,  -0.473*u.ns,   -0.080*u.ns,  0.500*u.ns,  0.002*u.ns,
  -1.017*u.ns,  0.000*u.ns,  -0.129*u.ns,  0.513*u.ns,   0.736*u.ns,  -0.466*u.ns,  0.963*u.ns,
  -0.359*u.ns,  0.000*u.ns,  -1.346*u.ns,  0.307*u.ns,   -2.019*u.ns,  -1.096*u.ns,  -1.012*u.ns,
  -1.032*u.ns,  0.000*u.ns,  -1.002*u.ns,  -0.860*u.ns,   0.630*u.ns,  -1.134*u.ns,  1.393*u.ns ]
         constants['t_8318']=[     0.000*u.ns,  0.000*u.ns,  -0.312*u.ns,  -0.498*u.ns,   0.336*u.ns,  -0.291*u.ns,  0.153*u.ns,
  -0.882*u.ns,  0.000*u.ns,  0.277*u.ns,  -0.753*u.ns,   -0.348*u.ns,  0.257*u.ns,  -0.237*u.ns,
  -0.957*u.ns,  0.000*u.ns,  -0.102*u.ns,  0.507*u.ns,   0.786*u.ns,  -0.420*u.ns,  0.987*u.ns,
  -0.365*u.ns,  0.000*u.ns,  -1.311*u.ns,  0.362*u.ns,   -2.152*u.ns,  -0.970*u.ns,  -0.725*u.ns,
  -0.982*u.ns,  0.000*u.ns,  -0.994*u.ns,  -0.895*u.ns,   0.454*u.ns,  -1.216*u.ns,  1.361*u.ns ]
         constants['t_8583']=[      0.000*u.ns,  0.000*u.ns,  -0.327*u.ns,  -0.494*u.ns,   0.226*u.ns,  -0.377*u.ns,  0.077*u.ns,
  -0.902*u.ns,  0.000*u.ns,  0.270*u.ns,  -0.751*u.ns,   -0.345*u.ns,  0.243*u.ns,  -0.259*u.ns,
  -0.937*u.ns,  0.000*u.ns,  -0.172*u.ns,  0.427*u.ns,   0.696*u.ns,  -0.478*u.ns,  0.916*u.ns,
  -0.355*u.ns,  0.000*u.ns,  -1.380*u.ns,  0.301*u.ns,   -1.998*u.ns,  -1.108*u.ns,  -1.020*u.ns,
  -1.021*u.ns,  0.000*u.ns,  -0.994*u.ns,  -0.892*u.ns,   0.591*u.ns,  -1.051*u.ns,  1.368*u.ns ]
         constants['t_8942']=[      0.000*u.ns,  0.000*u.ns,  -0.213*u.ns,  -0.431*u.ns,   0.276*u.ns,  -0.330*u.ns,  0.155*u.ns,
  -0.857*u.ns,  0.000*u.ns,  0.284*u.ns,  -0.734*u.ns,   -0.410*u.ns,  0.231*u.ns,  -0.269*u.ns,
  -1.586*u.ns,  0.000*u.ns,  0.567*u.ns,  1.171*u.ns,   1.438*u.ns,  0.180*u.ns,  1.604*u.ns,
  -0.269*u.ns,  0.000*u.ns,  -1.392*u.ns,  0.305*u.ns,   -2.027*u.ns,  -1.114*u.ns,  -1.019*u.ns,
  -0.941*u.ns,  0.000*u.ns,  -1.004*u.ns,  -0.897*u.ns,   0.607*u.ns,  -1.056*u.ns,  1.348*u.ns ]
         constants['t_9156']=[      0.000*u.ns,  0.000*u.ns,  -0.373*u.ns,  -0.552*u.ns,   0.075*u.ns,  -0.450*u.ns,  0.117*u.ns,
  -0.948*u.ns,  0.000*u.ns,  0.263*u.ns,  -0.762*u.ns,   -0.451*u.ns,  0.193*u.ns,  -0.296*u.ns,
  -0.990*u.ns,  0.000*u.ns,  -0.172*u.ns,  0.438*u.ns,   0.751*u.ns,  -0.470*u.ns,  0.897*u.ns,
  -0.417*u.ns,  0.000*u.ns,  -1.338*u.ns,  0.323*u.ns,   -2.035*u.ns,  -1.183*u.ns,  -0.603*u.ns,
  -1.115*u.ns,  0.000*u.ns,  -0.973*u.ns,  -0.854*u.ns,   0.591*u.ns,  -1.119*u.ns,  1.384*u.ns ]
         constants['t_9286']=[      0.000*u.ns,  0.000*u.ns,  -0.366*u.ns,  -0.542*u.ns,   0.121*u.ns,  -0.410*u.ns,  0.050*u.ns,
  -0.968*u.ns,  0.000*u.ns,  0.295*u.ns,  -0.730*u.ns,   -0.321*u.ns,  0.266*u.ns,  -0.245*u.ns,
  -0.989*u.ns,  0.000*u.ns,  -0.154*u.ns,  0.465*u.ns,   0.728*u.ns,  -0.477*u.ns,  0.974*u.ns,
  -0.399*u.ns,  0.000*u.ns,  -1.381*u.ns,  0.304*u.ns,   -1.991*u.ns,  -1.041*u.ns,  -0.609*u.ns,
  -1.142*u.ns,  0.000*u.ns,  -0.939*u.ns,  -0.823*u.ns,   0.687*u.ns,  -0.993*u.ns,  1.449*u.ns ]
         constants['t_9379']=[      0.000*u.ns,  0.000*u.ns,  -0.376*u.ns,  -0.528*u.ns,   0.150*u.ns,  -0.419*u.ns,  0.056*u.ns,
  -0.943*u.ns,  0.000*u.ns,  0.258*u.ns,  -0.739*u.ns,   -0.390*u.ns,  0.233*u.ns,  -0.241*u.ns,
  -0.988*u.ns,  0.000*u.ns,  -0.130*u.ns,  0.499*u.ns,   0.796*u.ns,  -0.498*u.ns,  0.953*u.ns,
  -0.391*u.ns,  0.000*u.ns,  -1.382*u.ns,  0.295*u.ns,   -1.967*u.ns,  -1.040*u.ns,  -0.673*u.ns,
  -1.113*u.ns,  0.000*u.ns,  -0.950*u.ns,  -0.838*u.ns,   0.682*u.ns,  -1.071*u.ns,  1.423*u.ns ]
         constants['t_9462']=[      0.000*u.ns,  0.000*u.ns,  -0.370*u.ns,  -0.528*u.ns,   0.056*u.ns,  -0.427*u.ns,  0.035*u.ns,
  -0.937*u.ns,  0.000*u.ns,  0.252*u.ns,  -0.752*u.ns,   -0.409*u.ns,  0.208*u.ns,  -0.295*u.ns,
  -0.971*u.ns,  0.000*u.ns,  -0.147*u.ns,  -0.516*u.ns,   0.678*u.ns,  -0.588*u.ns,  0.911*u.ns,
  -0.396*u.ns,  0.000*u.ns,  -1.337*u.ns,  0.302*u.ns,   -2.044*u.ns,  -1.074*u.ns,  -0.644*u.ns,
  -0.799*u.ns,  0.000*u.ns,  -1.269*u.ns,  -1.154*u.ns,   0.323*u.ns,  -1.328*u.ns,  1.106*u.ns ]
         constants['t_9613']=[      0.000*u.ns,  0.000*u.ns,  -0.359*u.ns,  -0.525*u.ns,   0.030*u.ns,  -0.418*u.ns,  0.036*u.ns,
  -0.957*u.ns,  0.000*u.ns,  0.278*u.ns,  -0.737*u.ns,   -0.375*u.ns,  0.236*u.ns,  -0.261*u.ns,
  -0.981*u.ns,  0.000*u.ns,  -0.140*u.ns,  -0.536*u.ns,   0.731*u.ns,  -0.598*u.ns,  0.902*u.ns,
  -0.387*u.ns,  0.000*u.ns,  -1.343*u.ns,  0.312*u.ns,   -2.000*u.ns,  -1.093*u.ns,  -0.768*u.ns,
  -0.738*u.ns,  0.000*u.ns,  -1.317*u.ns,  -1.200*u.ns,   0.319*u.ns,  -1.310*u.ns,  1.057*u.ns ]
         constants['t_9692']=[      0.000*u.ns,  0.000*u.ns,  -0.377*u.ns,  -0.526*u.ns,   0.041*u.ns,  -0.404*u.ns,  0.158*u.ns,
  -0.927*u.ns,  0.000*u.ns,  0.285*u.ns,  -0.755*u.ns,   -0.411*u.ns,  0.199*u.ns,  -0.284*u.ns,
  -0.973*u.ns,  0.000*u.ns,  -0.117*u.ns,  -0.486*u.ns,   0.754*u.ns,  0.000*u.ns,  0.941*u.ns,
  -0.381*u.ns,  0.000*u.ns,  -1.338*u.ns,  0.338*u.ns,   -2.007*u.ns,  -0.998*u.ns,  -0.618*u.ns,
  -0.784*u.ns,  0.000*u.ns,  -1.238*u.ns,  -1.000*u.ns,   0.292*u.ns,  -1.381*u.ns,  1.070*u.ns ]
         constants['t_9882']=[      0.000*u.ns,  0.000*u.ns,  -0.386*u.ns,  -0.539*u.ns,   0.037*u.ns,  -0.491*u.ns,  -0.024*u.ns,
  -1.197*u.ns,  0.000*u.ns,  0.499*u.ns,  -0.616*u.ns,   -0.273*u.ns,  0.402*u.ns,  -0.072*u.ns,
  -1.135*u.ns,  0.000*u.ns,  -0.016*u.ns,  0.584*u.ns,   0.871*u.ns,  -0.408*u.ns,  1.039*u.ns,
  -0.442*u.ns,  0.000*u.ns,  -1.381*u.ns,  0.339*u.ns,   -2.014*u.ns,  -1.068*u.ns,  -0.715*u.ns,
  -0.803*u.ns,  0.000*u.ns,  -1.330*u.ns,  -1.034*u.ns,   0.257*u.ns,  -1.478*u.ns,  1.000*u.ns ]
         constants['t_10012']=[      0.000*u.ns,  0.000*u.ns, -0.347*u.ns,  -0.546*u.ns,   0.124*u.ns,  -0.432*u.ns,  0.051*u.ns,
  -1.577*u.ns,  0.000*u.ns,  0.886*u.ns,  -0.188*u.ns,   0.229*u.ns,  0.858*u.ns,  0.286*u.ns,
  -1.119*u.ns,  0.000*u.ns,  -0.005*u.ns,  0.582*u.ns,   0.875*u.ns,  -0.390*u.ns,  1.059*u.ns,
  -0.406*u.ns,  0.000*u.ns,  -1.319*u.ns,  0.337*u.ns,   -2.012*u.ns,  -1.059*u.ns,  -0.618*u.ns,
  -0.825*u.ns,  0.000*u.ns,  -1.205*u.ns,  -0.952*u.ns,   0.241*u.ns,  -1.363*u.ns,  1.123*u.ns ]
#2025
         constants['t_10423']=[      0.000*u.ns,  0.000*u.ns,  -0.337*u.ns,  -0.509*u.ns,   0.125*u.ns,  -0.442*u.ns,  -0.049*u.ns,
  -0.936*u.ns,  0.000*u.ns,  0.238*u.ns,  -0.780*u.ns,   -0.484*u.ns,  0.172*u.ns,  -0.326*u.ns,
  -1.097*u.ns,  0.000*u.ns,  -0.028*u.ns,  0.543*u.ns,   0.764*u.ns,  -0.568*u.ns,  0.962*u.ns,
  -0.425*u.ns,  0.000*u.ns,  -1.339*u.ns,  0.324*u.ns,   -2.087*u.ns,  -1.170*u.ns,  -0.765*u.ns,
  -0.768*u.ns,  0.000*u.ns,  -1.312*u.ns,  -1.038*u.ns,   0.204*u.ns,  -1.518*u.ns,  0.960*u.ns]
         constants['t_11158']=[      0.000*u.ns,  0.000*u.ns,  -0.377*u.ns,  -0.542*u.ns,   0.115*u.ns,  -0.456*u.ns,  -0.184*u.ns,
  -0.982*u.ns,  0.000*u.ns,  0.233*u.ns,  -0.827*u.ns,   -1.809*u.ns,  0.228*u.ns,  -0.281*u.ns,
  -1.140*u.ns,  0.000*u.ns,  -0.020*u.ns,  0.606*u.ns,   0.813*u.ns,  -0.462*u.ns,  1.016*u.ns,
  -1.687*u.ns,  0.000*u.ns,  -0.101*u.ns,  1.552*u.ns,   -0.874*u.ns,  0.028*u.ns,  0.478*u.ns,
  -0.791*u.ns,  0.000*u.ns,  -1.310*u.ns,  -1.055*u.ns,   0.445*u.ns,  -1.393*u.ns,  0.794*u.ns]
         constants['t_11576']=[      0.000*u.ns,  0.000*u.ns,  -0.373*u.ns,  -0.541*u.ns,   0.143*u.ns,  -0.430*u.ns,  -0.117*u.ns,
  -0.939*u.ns,  0.000*u.ns,  0.213*u.ns,  -0.861*u.ns,   -0.396*u.ns,  0.231*u.ns,  -0.286*u.ns,
  -1.118*u.ns,  0.000*u.ns,  -0.005*u.ns,  0.569*u.ns,   0.823*u.ns,  -0.506*u.ns,  1.050*u.ns,
  -0.455*u.ns,  0.000*u.ns,  -1.324*u.ns,  0.337*u.ns,   -2.054*u.ns,  -1.458*u.ns,  -0.756*u.ns,
  -0.790*u.ns,  0.000*u.ns,  -1.279*u.ns,  -1.031*u.ns,   -0.108*u.ns,  -1.363*u.ns,  1.033*u.ns]
         constants['t_11676']=[    0.000*u.ns,  0.000*u.ns,  -0.374*u.ns,  -0.527*u.ns,   0.144*u.ns,  -0.406*u.ns,  -0.090*u.ns,
  -1.324*u.ns,  0.000*u.ns,  0.618*u.ns,  -0.929*u.ns,   -0.019*u.ns,  0.613*u.ns,  0.056*u.ns,
  -1.098*u.ns,  0.000*u.ns,  -0.045*u.ns,  0.584*u.ns,   0.915*u.ns,  -0.543*u.ns,  1.006*u.ns,
  -0.433*u.ns,  0.000*u.ns,  -1.322*u.ns,  0.322*u.ns,   -2.106*u.ns,  -1.526*u.ns,  -0.797*u.ns,
  -0.816*u.ns,  0.000*u.ns,  -1.254*u.ns,  -1.008*u.ns,   0.299*u.ns,  -1.388*u.ns,  1.048*u.ns]
         constants['t_11795']=[     0.000*u.ns,  0.000*u.ns,  -0.359*u.ns,  -0.524*u.ns,   0.124*u.ns,  -0.417*u.ns,  -0.084*u.ns,
  -1.327*u.ns,  0.000*u.ns,  0.628*u.ns,  -0.722*u.ns,   -0.167*u.ns,  0.608*u.ns,  0.140*u.ns,
  -1.078*u.ns,  0.000*u.ns,  -0.014*u.ns,  0.569*u.ns,   0.930*u.ns,  -0.501*u.ns,  1.000*u.ns,
  -0.417*u.ns,  0.000*u.ns,  -1.360*u.ns,  0.336*u.ns,   -2.079*u.ns,  -1.514*u.ns,  -0.775*u.ns,
  -0.736*u.ns,  0.000*u.ns,  -1.317*u.ns,  -1.076*u.ns,   0.296*u.ns,  -1.402*u.ns,  0.989*u.ns]
#
         scifi_time_aligment_consts = {
           2022: ['t_0', 't_4361','t_5117'],
           2023: ['t_0', 't_5478', 't_6208', 't_6443', 't_6677'],
           2024: ['t_0', 't_7649', 't_8318', 't_8583', 't_8942', 't_9156', 't_9286',
                  't_9379', 't_9462', 't_9613', 't_9692', 't_9882', 't_10012'],
           2025: ['t_0', 't_10423', 't_11158', 't_11576', 't_11676', 't_11795']
         }
         scifi_time_tags = scifi_time_aligment_consts[year]
         for c in scifi_time_tags:
          k=0
          for s in range(1,6):
            setattr(sGeo.Scifi,'station'+str(s)+c,constants[c][k])
            sGeo.Scifi['station'+str(s)+c] = constants[c][k]
            k+=1
            for p in ['H0','H1','H2','V0','V1','V2']:
               setattr(sGeo.Scifi,'station'+str(s)+p+c,constants[c][k])
               sGeo.Scifi['station'+str(s)+p+c] = constants[c][k]
               k+=1

         alignment={}
         alignment['t_0']=[
    0.00*u.um,  0.00*u.um,  0.00*u.um,
 0.00*u.um, 0.00*u.um,    0.00*u.um,
 0.00*u.um,  0.00*u.um,  0.00*u.um,
  0.00*u.um,  0.00*u.um,  0.00*u.um,
   0.00*u.um,   0.00*u.um,   0.00*u.um,
    0.00*u.um, 0.00*u.um,   0.00*u.um,
  0.00*u.um,   0.00*u.um,   0.00*u.um,
   0.00*u.um,  0.00*u.um,   0.00*u.um,
   0.00*u.um,   0.00*u.um,  0.00*u.um,
  0.00*u.um,  0.00*u.um,   0.00*u.um,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad]
         alignment['t_4361']=[
    7.30*u.um,  219.99*u.um,  247.73*u.um,
 -103.87*u.um, -105.64*u.um,    2.54*u.um,
 -286.76*u.um,  -53.99*u.um,  -85.45*u.um,
  103.99*u.um,  113.92*u.um,  148.52*u.um,
   -1.85*u.um,   78.98*u.um,   13.98*u.um,
    0.76*u.um, -109.75*u.um,   74.54*u.um,
  -16.79*u.um,   56.44*u.um,   96.94*u.um,
   71.04*u.um,  -64.13*u.um,   17.25*u.um,
   76.32*u.um,   51.34*u.um,  -13.33*u.um,
  -78.20*u.um,  158.73*u.um,   39.76*u.um,
   0.00*u.mrad,   -1.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad]
         alignment['t_4575']=[
  316.44*u.um,  521.82*u.um,  518.38*u.um,
 -226.06*u.um, -103.68*u.um,   65.08*u.um,
 -157.97*u.um,   59.67*u.um,  -12.36*u.um,
  -70.05*u.um,   63.72*u.um,   78.74*u.um,
   -1.85*u.um,   78.98*u.um,   13.98*u.um,
    0.76*u.um, -109.75*u.um,   74.54*u.um,
  -16.79*u.um,   56.44*u.um,   96.94*u.um,
   71.04*u.um,  -64.13*u.um,   17.25*u.um,
  172.36*u.um,   79.96*u.um,   57.20*u.um,
 -128.66*u.um,  104.65*u.um,  -31.24*u.um,
 0.00*u.mrad,   -1.17*u.mrad,    0.00*u.mrad,
 0.00*u.mrad,   -0.47*u.mrad,    0.00*u.mrad,
 0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
 0.00*u.mrad,   -0.28*u.mrad,    0.00*u.mrad,
 0.00*u.mrad,   -0.33*u.mrad,    0.00*u.mrad]
         alignment['t_4855']=[
  513.31*u.um,  775.65*u.um,  629.35*u.um,
 -339.98*u.um,  133.28*u.um,   83.79*u.um,
  162.48*u.um,  395.88*u.um,  297.98*u.um,
 -135.61*u.um,  144.02*u.um,   86.72*u.um,
   -1.85*u.um,   78.98*u.um,   13.98*u.um,
    0.76*u.um, -109.75*u.um,   74.54*u.um,
  -16.79*u.um,   56.44*u.um,   96.94*u.um,
   71.04*u.um,  -64.13*u.um,   17.25*u.um,
   83.73*u.um,   14.94*u.um,   33.91*u.um,
 -173.09*u.um, -184.25*u.um,   -8.56*u.um,
   0.00*u.mrad,   -1.68*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,   -0.68*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.08*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,   -0.37*u.mrad,    0.00*u.mrad]
         alignment['t_5172']=[
   331.99*u.um,  560.17*u.um,  460.83*u.um,
    13.37*u.um,  -57.78*u.um,  230.51*u.um,
  -136.69*u.um,   63.70*u.um,  -30.45*u.um,
     7.77*u.um,   -5.01*u.um,  156.92*u.um,
    -1.85*u.um,   78.98*u.um,   13.98*u.um,
     0.76*u.um, -109.75*u.um,   74.54*u.um,
   -16.79*u.um,   56.44*u.um,   96.94*u.um,
    71.04*u.um,  -64.13*u.um,   17.25*u.um,
    26.80*u.um,   51.14*u.um,  -31.93*u.um,
  -343.13*u.um,  -60.93*u.um, -260.49*u.um,
    0.00*u.mrad,   -1.00*u.mrad,    0.00*u.mrad,
    0.00*u.mrad,   -0.59*u.mrad,    0.00*u.mrad,
    0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
    0.00*u.mrad,    0.35*u.mrad,    0.00*u.mrad,
    0.00*u.mrad,   -0.32*u.mrad,    0.00*u.mrad]
         alignment['t_5431']=[       # 2023 emulsion run 4
   458.16*u.um,  658.14*u.um,  569.77*u.um,
  -285.21*u.um, -215.62*u.um,  -10.06*u.um,
  -131.42*u.um,   61.04*u.um,   18.21*u.um,
   -55.62*u.um,  -44.88*u.um,   60.99*u.um,
   -1.85*u.um,   78.98*u.um,   13.98*u.um,
    0.76*u.um, -109.75*u.um,   74.54*u.um,
  -16.79*u.um,   56.44*u.um,   96.94*u.um,
   71.04*u.um,  -64.13*u.um,   17.25*u.um,
   588.09*u.um,  500.82*u.um,  306.64*u.um,
  -429.88*u.um, -350.11*u.um, -408.64*u.um,
   0.00*u.mrad,   -3.06*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,   -1.34*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.81*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,   -0.30*u.mrad,    0.00*u.mrad]
         alignment['t_6443']=[       # 2023 emuslsion run 5
   705.43*u.um,  608.93*u.um,  822.19*u.um,
   697.92*u.um,  -64.00*u.um, -139.46*u.um,
   37.98*u.um,   74.00*u.um,  121.46*u.um,
   606.52*u.um,  137.93*u.um,   92.85*u.um,
  -1.85*u.um,  180.00*u.um,   13.98*u.um,
   100.76*u.um, -229.00*u.um, -104.00*u.um,
  -416.79*u.um, -156.00*u.um, -396.94*u.um,
   71.04*u.um,  -64.13*u.um,   17.25*u.um,
  -262.36*u.um,   54.92*u.um, -472.75*u.um,
  -562.60*u.um,  -45.39*u.um,  -78.87*u.um,
   0.00*u.mrad,   -3.88*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,   -1.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    0.00*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    2.27*u.mrad,    0.00*u.mrad,
   0.00*u.mrad,    2.99*u.mrad,    0.00*u.mrad]
         alignment['t_6677']=[       # 2023 ion run
   700.00*u.um, 895.00*u.um, 824.00*u.um,
  -373.00*u.um, -246.00*u.um, -266.00*u.um,
   161.00*u.um, 400.00*u.um, 287.00*u.um,
  -190.00*u.um, -59.80*u.um, -109.00*u.um,
  -2.05*u.um, 175.00*u.um,  12.00*u.um,
   0.77*u.um, -100.00*u.um,  77.50*u.um,
  -6.80*u.um, 134.00*u.um,  91.90*u.um,
   71.00*u.um, -64.10*u.um,  18.20*u.um,
   490.00*u.um, 573.00*u.um, 484.00*u.um,
  -547.00*u.um, -287.00*u.um, -298.00*u.um,
  0.00*u.mrad,  -4.55*u.mrad,   0.00*u.mrad,
  0.00*u.mrad,  -2.75*u.mrad,   0.00*u.mrad,
  0.00*u.mrad,   0.00*u.mrad,   0.00*u.mrad,
  0.00*u.mrad,   0.72*u.mrad,   0.00*u.mrad,
  0.00*u.mrad,  -0.10*u.mrad,   0.00*u.mrad]
# 2024
         alignment['t_7649']=[       # 2024 emuslsion run 8
   509.86*u.um, 732.95*u.um, 621.19*u.um,
  -10.56*u.um, 153.93*u.um, 92.22*u.um,
   94.54*u.um, 342.84*u.um, 168.64*u.um,
   164.88*u.um, 397.75*u.um, 246.37*u.um,
   185.00*u.um, 430.61*u.um, 245.85*u.um,
   0.76*u.um, 175.09*u.um, 166.33*u.um,
   0.00*u.um, 274.57*u.um, 185.82*u.um,
   50.00*u.um, 249.06*u.um, 175.30*u.um,
   313.78*u.um, 526.78*u.um, 361.02*u.um,
  -267.73*u.um, 339.17*u.um, 37.59*u.um,
   0.00*u.mrad, -0.61*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 0.34*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 0.00*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 0.20*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -0.55*u.mrad, 0.00*u.mrad]
         alignment['t_8318']=[       # 2024 emuslsion run 9
   469.66*u.um, 611.18*u.um, 693.42*u.um,
   201.35*u.um, 229.61*u.um, 103.33*u.um,
   60.22*u.um, 322.78*u.um, 264.75*u.um,
   496.09*u.um, 643.18*u.um, 472.68*u.um,
   234.71*u.um, 585.04*u.um, 382.37*u.um,
   529.26*u.um, 650.01*u.um, 614.34*u.um,
   431.02*u.um, 905.43*u.um, 748.38*u.um,
   116.27*u.um, 335.47*u.um, 201.41*u.um,
   100.54*u.um, 600.22*u.um, 269.97*u.um,
   550.18*u.um, 1248.48*u.um, 931.66*u.um,
   0.00*u.mrad, -3.02*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -0.95*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 0.12*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -1.55*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 2.31*u.mrad, 0.00*u.mrad]
         alignment['t_8583']=[       # 2024 emuslsion run 10
   611.7976*u.um, 889.86*u.um, 879.801*u.um,
   413.6267*u.um, 363.6332*u.um, 105.825*u.um,
   127.2448*u.um, 487.3149*u.um, 364.0717*u.um,
   707.2705*u.um, 849.2645*u.um, 589.3346*u.um,
   271.2779*u.um, 655.8296*u.um, 414.7906*u.um,
   686.1134*u.um, 851.719*u.um, 798.134*u.um,
   135.175*u.um, 614.5551*u.um, 442.9096*u.um,
   821.2312*u.um, 1189.1332*u.um, 1063.688*u.um,
   255.8865*u.um, 731.2859*u.um, 392.1363*u.um,
   507.8946*u.um, 1421.0582*u.um, 1139.182*u.um,
   0.00*u.mrad, -3.49*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -1.13*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 0.10*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 1.44*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 2.09*u.mrad, 0.00*u.mrad]
         alignment['t_8942']=[       # 2024 emuslsion run 11 /run_244
   271.8762*u.um,  484.1729*u.um,  471.3382*u.um,
   -21.0674*u.um,  -20.9474*u.um,  138.0984*u.um,
   -170.1168*u.um,  59.8273*u.um,  -23.7979*u.um,
   112.0014*u.um,  223.498*u.um,  271.4252*u.um,
   55.2461*u.um,  218.475*u.um,  111.7626*u.um,
   36.3226*u.um,  24.3772*u.um,  180.5263*u.um,
   268.446*u.um,  481.7177*u.um,  524.9105*u.um,
   -558.5981*u.um,  -483.5918*u.um,  -466.269*u.um,
   -27.5701*u.um,  42.2078*u.um,  12.4327*u.um,
   -245.3691*u.um,  211.5792*u.um,  79.8376*u.um,
   0.00*u.mrad, -0.94*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -0.08*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 0.007*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -2.88*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -0.19*u.mrad, 0.00*u.mrad]
         alignment['t_9156']=[       # 2024 emuslsion run 12 run_245
   532.3955*u.um, 869.4047*u.um, 920.2407*u.um,
   493.1367*u.um, 403.4604*u.um, 199.4534*u.um,
   174.5682*u.um, 580.0227*u.um, 463.4915*u.um,
   722.9943*u.um, 883.906*u.um, 626.2292*u.um,
   295.8365*u.um, 745.0445*u.um, 545.9911*u.um,
   810.2135*u.um, 996.7849*u.um, 935.6661*u.um,
   175.6937*u.um, 651.9452*u.um, 477.7067*u.um,
   928.1277*u.um, 1299.978*u.um, 1095.5873*u.um,
   565.4501*u.um, 1045.2995*u.um, 697.4744*u.um,
   528.7539*u.um, 1441.5199*u.um, 1156.4334*u.um,
   0.00*u.mrad, -3.35*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -1.14*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 0.23*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 1.51*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 1.74*u.mrad, 0.00*u.mrad]
         alignment['t_9286']=[       # 2024 emuslsion run 13 run_246
   398.3652*u.um, 498.1913*u.um, 1052.9471*u.um,
   717.8144*u.um, 21.1253*u.um, -522.9914*u.um,
   9.1061*u.um, 183.2849*u.um, 427.2521*u.um,
   1162.4342*u.um, 781.295*u.um, 401.2053*u.um,
   195.8365*u.um, 417.0663*u.um, 345.222*u.um,
   1250.2135*u.um, 1056.8695*u.um, 1098.8891*u.um,
   444.8639*u.um, 760.7313*u.um, 558.8336*u.um,
   1004.3348*u.um, 1031.0018*u.um, 1162.118*u.um,
   -5.4501*u.um, 361.4927*u.um, -126.8281*u.um,
   1628.7539*u.um, 2287.8624*u.um, 2539.903*u.um,
   0.00*u.mrad, -2.98*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -0.84*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 0.30*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, -1.79*u.mrad, 0.00*u.mrad,
   0.00*u.mrad, 2.00*u.mrad, 0.00*u.mrad]
         alignment['t_9379']=[     #2024 emulsion run 14 run_247
   821.8997*u.um, 45.1136*u.um, 898.2948*u.um,
   731.0633*u.um, -217.7475*u.um, -283.1271*u.um,
   402.9881*u.um, 26.3005*u.um, 541.7564*u.um,
   1274.5977*u.um, 738.074*u.um, 622.0383*u.um,
   334.8523*u.um, 348.9386*u.um, 575.9559*u.um,
   1474.6536*u.um, 1197.1878*u.um, 1265.0079*u.um,
   47.4766*u.um, 433.1232*u.um, 522.9466*u.um,
   1772.6823*u.um, 1918.6852*u.um, 1922.3606*u.um,
   182.9563*u.um, 884.8378*u.um, 662.6338*u.um,
   1847.6459*u.um, 2728.9236*u.um, 2585.8895*u.um,
   0*u.mrad, -3.4087*u.mrad, 0*u.mrad,
   0*u.mrad, -0.9179*u.mrad, 0*u.mrad,
   0*u.mrad, 0.2597*u.mrad, 0*u.mrad,
   0*u.mrad, 1.2555*u.mrad, 0*u.mrad,
   0*u.mrad, 2.154*u.mrad, 0*u.mrad]
         alignment['t_9462']=[     #2024 emulsion run 15 run_248
  -109.6284*u.um,  504.3064*u.um,  497.1011*u.um,
  -92.6082*u.um,  -92.2919*u.um,  103.0728*u.um,
  -343.0579*u.um,  166.6301*u.um,  100.8155*u.um,
  186.9365*u.um,  256.6612*u.um,  327.2862*u.um,
  59.6357*u.um,  335.2558*u.um,  265.7486*u.um,
  251.3367*u.um,  330.8842*u.um,  524.498*u.um,
  465.5392*u.um,  609.7781*u.um,  665.8652*u.um,
  -76.2475*u.um,  44.82*u.um,  115.7253*u.um,
  186.6246*u.um,  151.7738*u.um,  129.1633*u.um,
  368.1697*u.um,  974.7627*u.um,  815.4692*u.um,
   0*u.mrad, 0.92*u.mrad, 0*u.mrad,
   0*u.mrad, 1.38*u.mrad, 0*u.mrad,
   0*u.mrad, 1.32*u.mrad, 0*u.mrad,
   0*u.mrad, -1.36*u.mrad, 0*u.mrad,
   0*u.mrad, 1.03*u.mrad, 0*u.mrad]
         alignment['t_9613']=[     #2024 emulsion run 16 run_249
   347.9798*u.um,  485.1001*u.um,  602.751*u.um,
   47.9842*u.um,  144.8884*u.um,  205.1283*u.um,
   -0.6367*u.um,  318.2765*u.um,  263.1438*u.um,
   199.9615*u.um,  328.6094*u.um,  332.6156*u.um,
   26.9864*u.um,  479.7656*u.um,  295.4821*u.um,
   133.731*u.um,  142.5613*u.um,  256.4955*u.um,
   -471.9127*u.um,  218.7257*u.um,  81.9869*u.um,
   348.1659*u.um,  342.6864*u.um,  335.9995*u.um,
   334.7361*u.um,  1092.6102*u.um,  830.6452*u.um,
   18.9796*u.um,  287.9467*u.um,  99.387*u.um,
   0*u.mrad, 0.33*u.mrad, 0*u.mrad,
   0*u.mrad, 0.80*u.mrad, 0*u.mrad,
   0*u.mrad, 0.46*u.mrad, 0*u.mrad,
   0*u.mrad, 1.06*u.mrad, 0*u.mrad,
   0*u.mrad, -0.06*u.mrad, 0*u.mrad]
         alignment['t_9692']=[     #2024 emulsion run 17 run_2410 scifi 3v mat 3 is out
   486.3728*u.um,  745.5552*u.um,  696.4828*u.um,
   -84.1458*u.um,  129.8207*u.um,  212.3914*u.um,
   -144.0245*u.um,  166.041*u.um,  55.5924*u.um,
   221.8425*u.um,  441.3659*u.um,  494.7093*u.um,
   147.4787*u.um,  385.928*u.um,  193.8871*u.um,
   137.3001*u.um,  0*u.um,  303.9645*u.um,
   400.0489*u.um,  652.7457*u.um,  561.0253*u.um,
   -530.4546*u.um,  -507.6861*u.um,  -442.2345*u.um,
   -6.311*u.um,  162.2659*u.um,  39.5458*u.um,
   -143.7007*u.um,  203.3489*u.um,  80.8827*u.um,
   0*u.mrad, -1.73*u.mrad, 0*u.mrad,
   0*u.mrad, -0.06*u.mrad, 0*u.mrad,
   0*u.mrad, -0.23*u.mrad, 0*u.mrad,
   0*u.mrad, -3.51*u.mrad, 0*u.mrad,
   0*u.mrad, -0.43*u.mrad, 0*u.mrad]   
         alignment['t_9882']=[     #2024 emulsion run 18 run_2411
   271.3933*u.um,  511.5475*u.um,  553.8936*u.um,
   -203.5896*u.um,  -4.3931*u.um,  36.043*u.um,
   -141.547*u.um,  117.3645*u.um,  98.1091*u.um,
   70.3879*u.um,  275.8537*u.um,  242.826*u.um,
   25.8141*u.um,  230.3344*u.um,  133.4803*u.um,
   -2.9758*u.um,  96.0786*u.um,  207.7275*u.um,
   -50.0191*u.um,  147.4224*u.um,  131.0377*u.um,
   51.2951*u.um,  127.0692*u.um,  144.5879*u.um,
   209.5979*u.um,  329.4775*u.um,  186.2796*u.um,
   -148.5866*u.um,  268.9908*u.um,  87.7598*u.um,
   0*u.mrad, -0.88*u.mrad, 0*u.mrad,
   0*u.mrad, 0.26*u.mrad, 0*u.mrad,
   0*u.mrad, 0.10*u.mrad, 0*u.mrad,
   0*u.mrad, 0.06*u.mrad, 0*u.mrad,
   0*u.mrad, -0.22*u.mrad, 0*u.mrad]
         alignment['t_10012']=[     #2024 ion run run_2412
   152.0259*u.um, 447.3908*u.um, 381.2884*u.um,
   -528.6398*u.um, -125.534*u.um, -93.6233*u.um,
   -92.1306*u.um, 148.3639*u.um, 43.6692*u.um,
   89.2736*u.um, 331.7809*u.um, 299.4775*u.um,
   292.6668*u.um, 381.8102*u.um, 321.8736*u.um,
   401.2095*u.um, 381.4378*u.um, 521.3132*u.um,
   605.7078*u.um, 666.563*u.um, 784.4878*u.um,
   252.2577*u.um, 106.9793*u.um, 149.2466*u.um,
   334.8222*u.um, 235.9747*u.um, 407.865*u.um,
   1055.2959*u.um, 1153.5713*u.um, 1011.9634*u.um,
   0*u.mrad, -0.38*u.mrad, 0*u.mrad,
   0*u.mrad, 1.03*u.mrad, 0*u.mrad,
   0*u.mrad, 1.65*u.mrad, 0*u.mrad,
   0*u.mrad, -0.79*u.mrad, 0*u.mrad,
   0*u.mrad, 2.93*u.mrad, 0*u.mrad]
         alignment['t_10423']=[     #2025 emulsion run 19 run_251
   908.9748*u.um, 573.9855*u.um, 812.3022*u.um,
   86.541*u.um, 123.4473*u.um, 88.648*u.um,
   357.9727*u.um, 241.2571*u.um, 309.628*u.um,
   18.2807*u.um, 87.3165*u.um, 94.6991*u.um,
   -1.8476*u.um, 78.9774*u.um, 13.9775*u.um,
   100.7577*u.um, -59.748*u.um, 174.542*u.um,
   -100*u.um, 236.436*u.um, 196.9426*u.um,
   -11.044*u.um, -185.1257*u.um, 17.2473*u.um,
   -593.3074*u.um, -118.6736*u.um, -339.1444*u.um,
  -101.3628*u.um, -25.2405*u.um, 184.0245*u.um,
   0*u.mrad, -0.82*u.mrad, 0*u.mrad,
   0*u.mrad, -0.73*u.mrad, 0*u.mrad,
   0*u.mrad, 0*u.mrad, 0*u.mrad,
   0*u.mrad, -0.43*u.mrad, 0*u.mrad,
   0*u.mrad, 0.12*u.mrad, 0*u.mrad]
         alignment['t_11158']=[     #2025 emulsion run 20 run_252
   758.176*u.um, 84.0408*u.um, 221.625*u.um,
   126.9696*u.um, -100.0*u.um, -542.1658*u.um,
   56.0*u.um, -234.5533*u.um, -264.4273*u.um,
   210.2911*u.um, 130.0*u.um, 9.9216*u.um,
   -150.0*u.um, -102.0*u.um, -250.0*u.um,
   100.7577*u.um, -59.748*u.um, 304.0*u.um,
   -200.0*u.um, 256.0*u.um, 96.9426*u.um,
   -550.0*u.um, -600.0*u.um, 50*u.um,
   -892.3995*u.um, -108.0*u.um, -489.6239*u.um,
   -300.0*u.um, 129.0*u.um, 911.9577*u.um,
   0*u.mrad, -3.16*u.mrad, 0*u.mrad,
   0*u.mrad, -1.13*u.mrad, 0*u.mrad,
   0*u.mrad, 0*u.mrad, 0*u.mrad,
   0*u.mrad, -2.00*u.mrad, 0*u.mrad,
   0*u.mrad, 1.86*u.mrad, 0*u.mrad]
         alignment['t_11576']=[ #2025 emulsion run 21 run_253
   208.20*u.um, 125.00*u.um, 195.00*u.um,
   -8.53*u.um, 44.46*u.um, 52.20*u.um,
   -8.71*u.um, -30.53*u.um, -63.62*u.um,
   -98.60*u.um, 10.00*u.um, -45.00*u.um,
   -30.00*u.um, -8.00*u.um, -51.29*u.um,
   180.00*u.um, 98.05*u.um, 228.20*u.um,
   25.19*u.um, 67.28*u.um, 127.54*u.um,
   7.54*u.um, -141.72*u.um, -70.68*u.um,
   -5.00*u.um, 0.00*u.um, -50.29*u.um,
   -64.05*u.um, 86.94*u.um, 4.89*u.um,
   0.00*u.mrad, 0.01*u.mrad, -0.01*u.mrad,
   -0.47*u.mrad, -0.01*u.mrad, 0.08*u.mrad,
   0.90*u.mrad, 0.05*u.mrad, 0.00*u.mrad,
   -0.40*u.mrad, -0.01*u.mrad, 0.08*u.mrad,
   -0.23*u.mrad, 0.01*u.mrad, 0.04*u.mrad]
         alignment['t_11676']=[ #2025 emulsion run 22 run_254
   233.2526*u.um,  203.3113*u.um,  261.1019*u.um,
   -248.7328*u.um, -168.8227*u.um, -98.6278*u.um,
   -238.4141*u.um, -238.2084*u.um, -260.8086*u.um,
   72.2430*u.um, 152.4410*u.um, 162.6312*u.um,
   0.0000*u.um, -37.1101*u.um, -101.2731*u.um,
   130.0000*u.um, 13.4088*u.um, 150.0951*u.um,
   223.6887*u.um, 223.8778*u.um, 254.0594*u.um,
   -215.8022*u.um, -413.1773*u.um, -344.0464*u.um,
   -177.8921*u.um, -214.8997*u.um, -295.3263*u.um,
   125.5983*u.um, 255.9729*u.um, 212.1852*u.um,
   -0.4488*u.mrad, -0.3531*u.mrad, 0.3427*u.mrad,
   0.5384*u.mrad, 0.0341*u.mrad, 0.1195*u.mrad,
   0.7200*u.mrad, 0.2000*u.mrad, 0.0000*u.mrad,
   -1.0244*u.mrad, -0.5787*u.mrad, 0.0171*u.mrad,
   0.9128*u.mrad, 0.4085*u.mrad, 0.0171*u.mrad]
         alignment['t_11795']=[ #2025 emulsion run 23 run_255
   327.80*u.um, 296.15*u.um, 357.10*u.um,
   49.60*u.um, 102.00*u.um, 25.95*u.um,
   -81.10*u.um, -82.15*u.um, -114.40*u.um,
   21.55*u.um, 90.50*u.um, -4.55*u.um,
   -45.00*u.um, -36.60*u.um, -83.76*u.um,
   65.00*u.um, 2.20*u.um, 96.00*u.um,
   -126.80*u.um, -67.85*u.um, -94.90*u.um,
   29.05*u.um, -20.00*u.um, -29.05*u.um,
   400.60*u.um, 430.85*u.um, 331.80*u.um,
   -146.00*u.um, 119.80*u.um, 17.65*u.um,
   -0.24*u.mrad, -0.05*u.mrad, -0.30*u.mrad,
   -0.06*u.mrad, -0.18*u.mrad, -0.15*u.mrad,
   0.48*u.mrad, 0.00*u.mrad, 0.00*u.mrad,
   0.32*u.mrad, -0.28*u.mrad, 0.23*u.mrad,
   -0.20*u.mrad, 0.40*u.mrad, 1.32*u.mrad]

         scifi_spatial_aligment_consts = {
           2022: ['t_0', 't_4361','t_4575','t_4855','t_5172'],
           2023: ['t_0', 't_5431', 't_6443', 't_6677'],
           2024: ['t_0', 't_7649', 't_8318', 't_8583', 't_8942', 't_9156', 't_9286',
                  't_9379', 't_9462', 't_9613', 't_9692', 't_9882', 't_10012'],
           2025: ['t_0', 't_10423', 't_11158', 't_11576', 't_11676', 't_11795']
         }
         scifi_spatial_tags = scifi_spatial_aligment_consts[year]
         for c in scifi_spatial_tags:
          k=0
          for s in range(1,6):
           for o in range(0,2):
             for m in range(3):
               nr = s*100+10*o+m
               setattr(sGeo.Scifi,'LocM'+str(nr)+c,alignment[c][k])
               sGeo.Scifi['LocM'+str(nr)+c] = alignment[c][k]
               k+=1
          for s in range(1,6):
           for o in ["RotPhiS","RotPsiS","RotThetaS"]:
               setattr(sGeo.Scifi,o+str(s)+c,alignment[c][k])
               sGeo.Scifi[o+str(s)+c] = alignment[c][k]
               k+=1

         print('save',geoFileName)
         saveBasicParameters.execute(geoFileName,sGeo)
         

