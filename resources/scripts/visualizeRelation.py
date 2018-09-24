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


#now lets see if we can load the saved state
print "load"
cbX = IceHiveZ.ConnectorBlock.from_file(os.path.expandvars("$I3_BUILD/icehive_cb.dat"))

master_omkey = icetray.OMKey(1,1)


conindex = [-1] + range(0,6)

for c in conindex:
  cb0 = cbX.getConnector(c)
  print cb0.name
  rel0= cb0.relation

  acc = False
  for x in range(1, 87):
    acc_l = False
    l = [rel0.areRelated( master_omkey, icetray.OMKey(x,y)) for y in range(1, 30)]
    #print l
    for ll in l:
      acc_l = acc_l or ll
    acc = acc or acc_l
    
  if acc:
    for o in range(1, 61):
      line = ""  
      for s in range(1, 86+1):
        line += "x" if rel0.areRelated(master_omkey ,icetray.OMKey(s,o)) else "."
      print line
  else:
    print "---NONE---"
