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
from icecube.dataclasses import I3Constants

from icecube.phys_services.which_split import which_split

import argparse
parser = argparse.ArgumentParser(description='BUILD and SAVE a connector Block outlined by the configuration in this file')
parser.add_argument("-g", "--gcdfile", help="path to GCD file" ,type=str )
parser.add_argument("-i", "--infile", help="path for outfile", type=str )
parser.add_argument("-o", "--outfile", help="path for outfile", type=str )
parser.add_argument("-c", "--configfile", help="path for outfile", type=str )
args = parser.parse_args()

print "check access"
os.path.exists(os.path.expandvars(args.gcdfile))
os.path.exists(os.path.expandvars(args.infile))
os.path.exists(os.path.expandvars(args.configfile))
os.access(args.outfile, os.W_OK)

from icecube import IceHiveZ
#now lets see if we can load the saved connectorblock
print "load"
cb = IceHiveZ.ConnectorBlock.from_file(args.configfile)

icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)
#icetray.set_log_level_for_unit("I3HiveSplitter", icetray.I3LogLevel.LOG_DEBUG)
#icetray.set_log_level_for_unit("HiveSplitter", icetray.I3LogLevel.LOG_DEBUG)
#icetray.set_log_level_for_unit("I3HiveCleaning", icetray.I3LogLevel.LOG_DEBUG)
#icetray.set_log_level_for_unit("HiveCleaning", icetray.I3LogLevel.LOG_DEBUG)

tray = I3Tray()

#read some file
tray.AddModule("I3Reader","Reader",
        FilenameList=[args.gcdfile, args.infile])

tray.AddModule("I3HiveClustering","HiveClustering",
    Stream = icetray.I3Frame.Physics,
    InputName = "SRTTWOfflinePulses",
    OutputName = "HiveSRTTWOfflinePulses",
    Multiplicity = 3,
    MultiplicityTimeWindow = 1000*I3Units.ns,
    AcceptTimeWindow = 1000*I3Units.ns,
    RejectTimeWindow = 4000*I3Units.ns,
    MergeOverlap = 1,
    ConnectorBlock = cb)
#    If = lambda f: which_split('inice'))

tray.AddModule("I3Writer","Writer",
  Filename=args.outfile, 
  Streams=[icetray.I3Frame.DAQ, icetray.I3Frame.Physics])

tray.AddModule("TrashCan","End")
tray.Execute()
tray.Finish()
