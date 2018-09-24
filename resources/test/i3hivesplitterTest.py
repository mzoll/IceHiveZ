#!/usr/bin/env python 

"""
This is really not a unit test but a regression test, ensuring that this project's splitter continues
to produce exactly the same output as the original HiveSplitter. If any change is made to the
algorithm this test will fail, and if the change is intentional the test data will need to be regenerated
to put this test back on track.
This script can also be used to generate the testcase itself by specifying the number of output FramePackets as second argument,
 i.e. $ ./i3hivesplitterTest.py 100
"""
from icecube import icetray, dataclasses

class I3HiveSplitterVerifier(icetray.I3PacketModule):
  """An I3Module which compares that two different splitters produce exactly the same results"""
  def __init__(self, context):
    super(I3IceHiveVerifier, self).__init__(context, icetray.I3Frame.DAQ)
    self.packetCount = 0
    self.orgName = ""
    self.redoName = ""
    self.AddParameter("OriginalName", "Name of the original sub-eventstream", self.orgName)
    self.AddParameter("RedoName", "Name of the reprocessed sub-eventstream", self.redoName)
    self.AddOutBox("OutBox")
  def Configure(self):
    self.orgName = self.GetParameter("OriginalName")
    self.redoName = self.GetParameter("RedoName")
  def MarkAndPush(self, frames):
    frames[0].Put("DIFF", icetray.I3Bool(True))
    for frame in frames:
      self.PushFrame(frame)
  def FramePacket(self, frames):
    self.packetCount+=1
    
    qframe = frames[0]
    
    #splitcount needs to be equal
    if (qframe.Get(self.orgName+"SplitCount").value != qframe.Get(self.redoName+"SplitCount").value):
      icetray.logging.log_error("Different number of delivered Splits")
      self.MarkAndPush(frames)
      return
    
    orgSplits = []
    redoSplits = []
    
    for frame in frames:
      if frame["I3EventHeader"].sub_event_stream == self.orgName:
        orgSplits.append(frame)
      elif frame["I3EventHeader"].sub_event_stream == self.redoName:
        redoSplits.append(frame)
    
    #first, there need to be the same number of frames
    if (len(orgSplits) != len(redoSplits) ):
      icetray.logging.log_error("Regression: Different numbers of split frames in packet "+str(self.packetCount)+":\n"\
        +" original="+str(len(orgSplits))+", redo="+str(len(redoSplits)))
      self.MarkAndPush(frames)
      
    #next, step through the split frames in parallel and make sure that the the split pulse series are identical
    #compare these objects:
    # - RecoPulseSeries
        
    for frameCount in xrange(len(orgSplits)):
      orgMap = dataclasses.I3RecoPulseSeriesMap.from_frame(orgSplits[frameCount], self.orgName)
      redoMap = dataclasses.I3RecoPulseSeriesMap.from_frame(redoSplits[frameCount], self.redoName)
    
      #should be the same number of DOMs present in both maps
      if (len(orgMap) != len(redoMap)):
        icetray.logging.log_error("Regression: Different numbers of hit DOMs in packet "+str(self.packetCount)+", split frame "+str(frameCount)+":\n"\
          +" original= "+str(len(orgMap))+", current="+str(len(redoMap)))
        self.MarkAndPush(frames)
        return
      
      #iterate over all of the DOMs and ensure that the same DOMs are hit, and have the same pulses
      for k in xrange(len(orgMap)):
        #check that both pulse series are for the same DOM
        orgOMKey = orgMap.keys()[k]
        redoOMKey = redoMap.keys()[k]
        if (orgOMKey != redoOMKey):
          icetray.logging.log_error("Regression: Missmatched DOMs in packet "+str(self.packetCount)+", split frame "+str(frameCount)+":\n"\
            +" original= "+str(orgOMKey)+", current="+str(redoOMKey))
          self.MarkAndPush(frames)
          return
        #check that the DOM has the same number of pulses in both series
        orgPulses = orgMap.values()[k]
        redoPulses = redoMap.values()[k]
        if (len(orgPulses) != len(redoPulses)):
          icetray.logging.log_error("Regression: Different numbers of hits on DOM "+str(orgOMKey)+" in packet "+str(self.packetCount)+", split frame "+str(frameCount)+":\n"\
            +" original= "+str(len(orgPulses))+", current="+str(len(redoPulses)))
          self.MarkAndPush(frames)
          return
        
        #iterate over all of the pulses and check that they are the same
        for p in xrange(len(orgPulses)):
          orgPulse = orgPulses[p]
          redoPulse = redoPulses[p]
          if (len(orgPulses) != len(redoPulses)):
            icetray.logging.log_error("Regression: Different pulse "+str(p)+" on DOM "+str(orgOMKey)+" in packet "+str(self.packetCount)+", split frame "+str(frameCount)+"!")
            self.MarkAndPush(frames)
            return
    for frame in frames:
      self.PushFrame(frame)

class FrameDelivery(icetray.I3PacketModule):
  """A module that just deliveres a specified number of packages"""
  def __init__(self, context):
    super(FrameDelivery, self).__init__(context, icetray.I3Frame.DAQ)
    self.AddParameter("NRequested", "Deliver that many frame-packes", 100)
    self.AddOutBox("OutBox")
  def Configure(self):
    self.nrequested = self.GetParameter("NRequested")
    self.ndelivered = 0
  def FramePacket(self, frames):
    if (self.ndelivered<self.nrequested):
      for frame in frames:
        self.PushFrame(frame)
      self.ndelivered+=1
    else:
      self.RequestSuspension()
    
import unittest
#define when there should a loud screem
class TestEqual(unittest.TestCase):
  """ define what the test-case should actually see """
  def testSequence(self):
    if (self.frame.Stop == icetray.I3Frame.DAQ):
      self.assert_(not self.frame.Has("DIFF"))

#tray ==============================================
import sys
from icecube import dataio
from I3Tray import *

from icecube import IceHive

#running this with with a number
if (len(sys.argv) ==1):
  test = True
elif (len(sys.argv) ==2):
  test = False
else:
  raise RuntimeError("specify either a single number or no option at all")

#find testdata
i3_testdata = (os.path.expandvars("$I3_TESTDATA"))


tray = I3Tray()

if (test):
  tray.AddModule("I3Reader","Reader",
    Filename = os.path.join(i3_testdata,"IceHiveZ/i3hivesplitter_testcase.i3.bz2"))
else:
  tray.AddModule("I3Reader","Reader",
    filenamelist = ["/data/exp/IceCube/2011/filtered/level2/0513/Level2_IC86.2011_data_Run00118178_0513_GCD.i3.gz",
                    "/data/exp/IceCube/2011/filtered/level2/0513/Level2_IC86.2011_data_Run00118178_Part00000000.i3.bz2"])

  tray.AddModule(lambda f: not (f.Stop==icetray.I3Frame.Physics and (f["I3EventHeader"].sub_event_stream not in ["redo", "org"]) ))
  
  tray.AddModule("Keep", "keep",
                 Keys = ["I3Geometry",
                         #"I3Calibration",
                         #"I3DetectorStatus",
                         "I3TriggerHierarchy",
                         "CleanTriggerHierarchy",
                         "FilterMask",
                         "I3EventHeader",
                         "I3DST11",
                         "I3SuperDST",
                         "OfflinePulses",
                         "MaskedOfflinePulses"])

#allow a unique name to the modules
if (test):
  redo_org = '_redo'
else:
  redo_org = '_org'

tray.AddModule("I3HiveSplitter","HiveSplitter",
  SubEventStreamName = "IceHiveSplit",
  InputName = "OfflinePulses",
  OutputName = "SplitOfflinePulses"+redo_org,
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

  
if (test):
  tray.AddModule(I3IceHiveVerifier,"SplitVerifier",
                 OriginalName = "SplitOfflinePulses"+"_org",
                 RedoName = "SplitOfflinePulses"+"_redo")

  tray.AddModule(icetray.I3TestModuleFactory(TestEqual), "TestEqual",
                Streams=[icetray.I3Frame.DAQ])

else:
  tray.AddModule(FrameDelivery, "deliver",
                 NRequested = int(sys.argv[1]))
  
  tray.AddModule("I3Writer", "Writer",
                  Filename= os.path.join(os.path.expandvars("$I3_BUILD"),"IceHiveZ/resources/i3hivesplitter_testcase.i3.bz2"),
                  Streams = [icetray.I3Frame.Geometry,
                            #icetray.I3Frame.Calibration,
                            #icetray.I3Frame.DetectorStatus,
                            icetray.I3Frame.DAQ,
                            icetray.I3Frame.Physics])
                  
tray.AddModule("TrashCan","last")

tray.Execute()
tray.Finish()

