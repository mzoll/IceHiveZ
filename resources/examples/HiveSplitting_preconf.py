#!/usr/bin/env python

# script

import os, sys, getopt, glob
from os.path import expandvars
from icecube import icetray, dataio, dataclasses
from optparse import OptionParser
from icecube.icetray import I3Units

from I3Tray import *

class RunParameters:
  def __init__(self):
    self.Infile = ''
    self.Outfile = ''
    self.GCDfile = ''
    self.CB = ''
    self.NEvents = 0

@icetray.traysegment
def Process(tray, name, **kwargs):
  #tray.AddModule("Dump", "dump")
 
  from icecube import IceHiveZ
  conBlock = IceHiveZ.ConnectorBlock.from_file(params.CB)
 
  tray.AddModule("I3HiveSplitter","IceHiveSplit",
    InputName = "OfflinePulses",
    OutputName = "Split",
    Multiplicity = 3,
    MultiplicityTimeWindow = 1000*I3Units.ns,
    AcceptTimeWindow = 1000*I3Units.ns,
    RejectTimeWindow = 4000*I3Units.ns,
    MergeOverlap = 1,
    ConnectorBlock = conBlock,
    SaveSplitCount = True,
    UpdateTriggerHierarchy = True,
    TrigHierName = "I3TriggerHierarchy", # "QTriggerHierarchy",                                                                                                                      
    ReadoutWindowMinus = 4000.*I3Units.ns,
    ReadoutWindowPlus = 6000.*I3Units.ns)

#___________________PARSER__________________________
def parseOptions(parser, params):
  parser.add_option("-i", "--input", action="store", type="string", default="", dest="INPUT", help="Input i3 file to process")
  parser.add_option("-o", "--output", action="store", type="string", default="", dest="OUTPUT", help="Output i3 file")
  parser.add_option("-g", "--gcd", action="store", type="string", default="", dest="GCD", help="GCD file for input i3 file")
  parser.add_option("-c", "--cb", action="store", type="string", default="", dest="CB", help="ConnectionBlock configuration file")
  parser.add_option("-n", "--nevents", action="store", type="int", default=0, dest="NEVENTS", help="Number of Events to process")
  (options,args) = parser.parse_args()
  params.Infile = options.INPUT
  params.Outfile = options.OUTPUT
  params.GCDfile = options.GCD
  params.CB = options.CB
  params.NEvents = options.NEVENTS

#___________________IF STANDALONE__________________________
if (__name__=='__main__'):
  from optparse import OptionParser

  params = RunParameters()

  usage = 'usage: %prog [options]'
  parser = OptionParser(usage)

  parseOptions(parser, params)

  Infile_List = glob.glob(params.Infile)

  from icecube import icetray, dataio

  tray = I3Tray()

  tray.AddModule("I3Reader", "reader",
    filenamelist=[params.GCDfile]+Infile_List)

  tray.AddSegment(Process, "process", params=params )

  if params.Outfile:
    tray.AddModule("I3Writer","writer",
      streams = [icetray.I3Frame.DAQ, icetray.I3Frame.Physics],
      filename = params.Outfile,)

  tray.AddModule("TrashCan","trashcan")

  if (params.NEvents==0):
    tray.Execute()
  else:
    tray.Execute(params.NEvents)

  tray.Finish()
