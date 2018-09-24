#!/usr/bin/env python

FAST = False

from argparse import ArgumentParser
parser = ArgumentParser(description=__doc__)
parser.add_argument('infile', nargs='*')
parser.add_argument('outfile')
opts = parser.parse_args()

from icecube import icetray, dataclasses, dataio
from glob import glob
import os
from I3Tray import I3Tray, I3Units

tray = I3Tray()

from icecube import phys_services, hitspool_reader, DomTools
Om2MbIdKeyFile = os.path.expandvars("$I3_SRC") + "/phys-services/resources/mainboard_ids.xml.gz"
tray.AddService("I3XMLOMKey2MBIDFactory","keys",Infile=Om2MbIdKeyFile)

p = "/scratch/jvansanten/hitspool/ANON_20160221_072723_ichub%02d.sps.icecube.southpole.usap.gov/*.dat" #only on cobalt08
files = []
for i in range(01,87): #for DOMhubs 01 to 86
  hubfiles = glob(p%i)
  hubfiles.sort()
  files.append(hubfiles)

if len(files[0]):
  icetray.logging.log_info("found %d files on %d DOM-hubs worth of hitspool data"%(len(files[0]), len(files)) )
#else:
#  icetray.logging.log_fatal("no suitable files found")

f = dataio.I3File(os.path.expandvars("$I3_GCD"))
while f.more():
  gcdframe= f.pop_frame()

geo = gcdframe['I3Geometry']
calib = gcdframe['I3Calibration']
status = gcdframe['I3DetectorStatus']

from icecube import IceHiveZ

hasher = IceHiveZ.CompactOMKeyHashService(geo)

ht_params = IceHiveZ.HiveTrigger_ParameterSet()
ht_params.multiplicity = 3
ht_params.multiplicityTimeWindow = 1000*I3Units.ns
ht_params.acceptTimeWindow = 1000*I3Units.ns
ht_params.rejectTimeWindow = 4000*I3Units.ns
if not FAST:
  ht_params.connectionBlock = IceHiveZ.DefaultCB_Splitter().buildConnectorBlock(geo, calib, status)
else:
  ht_params.connectionBlock = IceHiveZ.ConnectorBlock(hasher)
  connection = IceHiveZ.BoolConnection(hasher)
  connection.connect_everything = True
  relation = IceHiveZ.Relation(hasher, True)
  connector = IceHiveZ.Connector("Fake", hasher, connection, relation) 
  ht_params.connectionBlock.addConnector(connector)


tray.AddModule("HitSpoolReader","read",
        Trigger= IceHiveZ.IceHiveTrigger(ht_params),
        InputFilePaths=files,
        Output='InIceRawData',
        MainboardIDMapping="I3OMKey2MBID",
        BadDomsList=,badDomsList, 
        Year=2016,
        JunkSize =100)

#tray.Add("Dump")

if opts.outfile:
  tray.Add("I3Writer", Streams=map(icetray.I3Frame.Stream, 'QP'),
      # DropOrphanStreams=[icetray.I3Frame.DAQ],
      filename=opts.outfile)

tray.Execute(10000)
tray.Finish()
