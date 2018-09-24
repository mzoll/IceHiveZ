#!/usr/bin/env python

"""
This script demonstrates running the two central modules of IceHiveZ: 
HiveSplitter to split a RecoPulseSeries (located in the Q frame) into separate pulse series
for each subevent, each in its own P frame.
HiveCleaning to remove outlier hits, cleaning on the pulse-series
Finally make an reconstruction on the purged PulseSeries
"""

import sys, os
from I3Tray import *
from icecube import icetray, dataio, dataclasses

from icecube.icetray import I3Units

i3_testdata = os.path.expandvars("$I3_TESTDATA")

print ("Output will be written to "+os.path.join(os.path.expandvars("$I3_BUILD")+"/IceHiveZ/resources","example.i3.bz2"))

icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)
#icetray.set_log_level_for_unit("I3HiveSplitter", icetray.I3LogLevel.LOG_DEBUG)
#icetray.set_log_level_for_unit("HiveSplitter", icetray.I3LogLevel.LOG_DEBUG)
#icetray.set_log_level_for_unit("I3HiveCleaning", icetray.I3LogLevel.LOG_DEBUG)
#icetray.set_log_level_for_unit("HiveCleaning", icetray.I3LogLevel.LOG_DEBUG)

tray = I3Tray()

#read some file
tray.AddModule("I3Reader","Reader",
        FilenameList=[os.path.join(i3_testdata+"/IceHiveZ","example_scenario.i3.bz2")])
#remove all P frames and thereby reduce it
tray.AddModule(lambda f: not f.Stop == icetray.I3Frame.Physics)

#remove keys which might possibly collide
tray.AddModule("Delete", "delete_keys_in_use",
               Keys = ["MaskedOfflinePulses"+"_Physics",
                       "MaskedOfflinePulses"+"_Noise",
                       "MaskedOfflinePulses"+"_Noised",
                       "IceHiveSplit"+"SplitCount"])

#tray.AddModule("Dump", "dump")

from icecube import IceHiveZ

tray.AddModule("I3HiveSplitter","HiveSplitter",
  SubEventStreamName = "IceHiveSplit",
  InputName = "OfflinePulses",
  OutputName = "SplitOfflinePulses",
  Multiplicity = 3,
  MultiplicityTimeWindow = 1000*I3Units.ns,
  AcceptTimeWindow = 1000*I3Units.ns,
  RejectTimeWindow = 4000*I3Units.ns,
  MergeOverlap = 1,
  ConfiguratorBlock = IceHiveZ.DefaultCB_Splitter(add_DC=True),
  SaveSplitCount = True,
  UpdateTriggerHierarchy = True,
  TrigHierName = "I3TriggerHierarchy", # "QTriggerHierarchy",                                                                                                                      
  ReadoutWindowMinus = 4000.*I3Units.ns,
  ReadoutWindowPlus = 6000.*I3Units.ns)
"""
#clean the output pulses in the P-frame
tray.AddModule("I3HiveCleaning","HiveClean",
  InputName = "SplitOfflinePulses",
  OutputName = "HC"+"MaskedOfflinePulses",
  Multiplicity = 1,
  MaxTimeResidualEarly = -float("inf"),
  MaxTimeResidualLate = float("inf"),
  ConfiguratorBlock = IceHiveZ.DefaultCB_Clean(),
  Stream = icetray.I3Frame.Physics)

#make a reconstruction in the clean Pulses in the P frame
from icecube import improvedLinefit
tray.AddSegment(improvedLinefit.simple, 'LineFit_HC',
  inputResponse = "HCSplitOfflinePulses",
  fitName = "LineFit",
  If = lambda f: True)

tray.AddModule("I3Writer","Writer",
  Filename=os.path.join(os.path.expandvars("$I3_BUILD")+"/IceHiveZ/resources","example.i3.bz2"),
  Streams=[icetray.I3Frame.Physics, icetray.I3Frame.DAQ])
"""
tray.AddModule("TrashCan","End")
tray.Execute()
tray.Finish()
