#!/usr/bin/env python

"""
This script demonstrates running HiveSplitter to split a
RecoPulseSeries (located in the Q frame) into separate pulse series
for each subevent, each in its own P frame.
USAGE: python [this_script.py] [infile] [outfile]
"""

import sys
from I3Tray import *
from icecube import icetray, dataio, dataclasses
from icecube import IceHiveZ

from icecube.icetray import I3Units

icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)
icetray.set_log_level_for_unit("I3IceHive", icetray.I3LogLevel.LOG_DEBUG)

tray = I3Tray()

tray.AddModule("I3Reader","Reader",
        FilenameList=sys.argv[1:-1])

tray.AddModule("Delete", "delete_keys_in_use",
               Keys = ["MaskedOfflinePulses"+"_Physics",
                       "MaskedOfflinePulses"+"_Noise",
                       "MaskedOfflinePulses"+"_Noised",])

#tray.AddModule("Dump", "dump")

tray.AddModule("I3HiveClustering","IceHiveSplit",
  InputName = "OfflinePulses",
  OutputName = "HSOfflinePulses",
  Multiplicity = 3,
  MultiplicityTimeWindow = 1000*I3Units.ns,
  AcceptTimeWindow = 1000*I3Units.ns,
  RejectTimeWindow = 4000*I3Units.ns,
  MergeOverlap = 1,
  ConfiguratorBlock = IceHiveZ.DefaultCB_Splitter(add_DC=True),
  Stream = icetray.I3Frame.Physics,
  If = lambda f: True)

tray.AddModule("I3Writer","Writer",
  Filename=sys.argv[-1],
  Streams=[icetray.I3Frame.Physics, icetray.I3Frame.DAQ])

tray.AddModule("TrashCan","End")
tray.Execute()
tray.Finish()
