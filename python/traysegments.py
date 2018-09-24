"""
defines traysegements for the use of IceHiveZ
"""
from icecube import icetray
from icecube.icetray import I3Units
from icecube.dataclasses import I3Constants


@icetray.traysegement
def IceHiveZ_Aggressive(tray, name,
                        InputPulses,
                        SplitPulses,
                        CleanedPulses):
  """
  Perform the hardest agressive splitting that is available;
  You'll probably get fractured up events and some noise-clusters,
  which need to be treated, for example with CoincidentSuite
  """
  from icecube import IceHiveZ
    
  tray.AddModule("I3HiveSplitter","IceHiveZ",
    InputName = InputPulses,
    OutputName = SplitPulses,
    Multiplicity = 3,
    MultiplicityTimeWindow = 1000*I3Units.ns,
    AcceptTimeWindow = 1000*I3Units.ns,
    RejectTimeWindow = 4000*I3Units.ns,
    MergeOverlap = 1,
    HiveConfigurationBlock = IceHiveZ.DefaultHCB_Splitter(),
    SaveSplitCount = True,
    UpdateTriggerHierarchy = True,
    ReadoutWindowMinus = 4000.*I3Units.ns)
    ReadoutWindowPlus = 6000.*I3Units.ns)

  tray.AddModule("I3HiveCleaning","HiveCleanZ",
    InputName = SplitPulses,
    OutputName = CleanedPulses,
    Multiplicity = 1,
    MaxTimeResidualEarly = -float("inf"),
    MaxTimeResidualLate =  float("inf"),
    HiveConfigurationBlock = IceHiveZ.DefaultHCB_Clean(),
    Stream = iceetray.I3Frame.Physics)