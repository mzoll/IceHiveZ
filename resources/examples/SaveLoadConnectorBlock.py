#!/usr/bin/env python

"""
Because the GCD does not change much over the course of a datataking season, it makes sense to generate the ConnectionBlock
once and save it to file, in order to load it later and run the splitter
"""

import os, sys, getopt, glob
from os.path import expandvars
from icecube import icetray, dataio, dataclasses
from optparse import OptionParser
from icecube.icetray import I3Units

from icecube import IceHiveZ

#we need the full GCD information
#f = dataio.I3File(os.path.expandvars("$I3_GCD"))
f = dataio.I3File("/opt/i3/data/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz")
while f.more():
  gcdframe= f.pop_frame()

geo = gcdframe["I3Geometry"]
calib = gcdframe["I3Calibration"]
status = gcdframe["I3DetectorStatus"]

#make a ConfigurationBlock by generating one, or loading one
print "construct"
hcb = IceHiveZ.DefaultCB_Splitter(add_DC=True)

#build the ConnectorBlock by convolution the ConfigurationBlock with GCD information
print "configure"
cb = hcb.buildConnectorBlock(geo, calib, status)

#save the block to a file from which it can be loaded later;
#only essential information will be save
print "save"
cb.to_file("icehive_cb.dat")

#now lets see if we can load the saved state
print "load"
cbX = IceHiveZ.ConnectorBlock.from_file("icehive_cb.dat")

#this can than be fed to the HiveSplitter in a tray
print "tray"
from I3Tray import *
from icecube import icetray, dataio

tray = I3Tray()

tray.AddModule("I3HiveSplitter","IceHiveSplit",
    SubEventStreamName = "IceHiveSplit",
    InputName = "OfflinePulses",
    OutputName = "Split",
    Multiplicity = 3,
    MultiplicityTimeWindow = 1000*I3Units.ns,
    AcceptTimeWindow = 1000*I3Units.ns,
    RejectTimeWindow = 4000*I3Units.ns,
    MergeOverlap = 1,
    ConnectorBlock = cbX,
    SaveSplitCount = True,                                                                                                     
    ReadoutWindowMinus = 4000.*I3Units.ns,
    ReadoutWindowPlus = 6000.*I3Units.ns)

tray.Finish()
