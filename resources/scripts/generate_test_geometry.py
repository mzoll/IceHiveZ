#!/usr/bin/env python
 
"""
Generate a test geometry
"""

from argparse import ArgumentParser
parser = ArgumentParser(description=__doc__)
parser.add_argument('outfile')
opts = parser.parse_args()

import sys
from I3Tray import *
from icecube import icetray, dataio, dataclasses
from icecube import IceHive

tray = I3Tray()

tray.AddModule("I3GeoDeliver", "I3GeoDeliver")
tray.AddModule("I3Writer","writer",
  Filename = opts.outfile,
  Streams = [icetray.I3Frame.Geometry])

tray.AddModule("TrashCan","can")
tray.Execute()
tray.Finish()