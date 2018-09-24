#!/usr/bin/env python

import os
from icecube import icetray, dataclasses, dataio

f = dataio.I3File(os.path.expandvars("$I3_GCD"))
while f.more():
  gcdframe= f.pop_frame()

geo = gcdframe['I3Geometry']
calib = gcdframe['I3Calibration']
status = gcdframe['I3DetectorStatus']

from icecube import IceHiveZ

hasher = IceHiveZ.CompactOMKeyHashService(geo)

connectorBlock = IceHiveZ.ConnectorBlock(hasher)

connection = IceHiveZ.BoolConnection(hasher)
connection.connect_everything = True
relation = IceHiveZ.Relation(hasher, True)
connector = IceHiveZ.Connector("Fake", hasher, connection, relation) 
connectorBlock.addConnector(connector)
