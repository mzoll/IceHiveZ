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
from icecube import IceHive

from icecube.icetray import I3Units

icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)
icetray.set_log_level_for_unit("I3HiveCleaning", icetray.I3LogLevel.LOG_DEBUG)
tray = I3Tray()

tray.AddModule("I3Reader","Reader",
        FilenameList=sys.argv[1:-1])

tray.AddModule("Delete", "delete_keys_in_use",
               Keys = ["HCMaskedOfflinePulses",])

#tray.AddModule("Dump", "dump")

from icecube import IceHiveZ

tray.AddModule("I3HiveCleaning","HiveClean",
  InputName = "MaskedOfflinePulses",
  OutputName = "HC"+"MaskedOfflinePulses",
  Multiplicity = 1,
  MaxTimeResidualEarly = -float("inf"),
  MaxTimeResidualLate = float("inf"),
  HiveConfigurationBlock = IceHiveZ.DefaultCB_Clean(),
  Stream = icetray.I3Frame.Physics,
  If = lambda f: True)

tray.AddModule("I3Writer","Writer",
  Filename=sys.argv[-1],
  Streams=[icetray.I3Frame.Physics, icetray.I3Frame.DAQ])

tray.AddModule("TrashCan","End")
tray.Execute()
tray.Finish()
