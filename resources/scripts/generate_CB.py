#!/usr/bin/env python

"""
Because the GCD does not change much over the course of a datataking season, it makes sense to generate the ConnectionBlock
once and save it to file, in order to load it later and run the splitter later
"""

import os, sys, getopt, glob
from os.path import expandvars
from icecube import icetray, dataio, dataclasses
from optparse import OptionParser
from icecube.icetray import I3Units

#=== ARGUMENT PARSING

from argparse import ArgumentParser
parser = ArgumentParser(description=__doc__)
parser.add_argument('gcdfile')
parser.add_argument('outfile')
opts = parser.parse_args()

#== BODY

from icecube import IceHiveZ

hcb = IceHiveZ.DummyHCB(add_DC=True)

#we need the full GCD information
#f = dataio.I3File(os.path.expandvars("$I3_GCD"))
f = dataio.I3File(opts.gcdfile)
while f.more():
  gcdframe= f.pop_frame()




#build the connection block
cb = hcb.buildConnectionBlock(gcdframe["I3Geometry"], gcdframe["I3Calibration"], gcdframe["I3DetectorStatus"])

#save the block to a file from which it can be loaded later;
#only essential information will be save
cb.to_file(opts.outfile)
