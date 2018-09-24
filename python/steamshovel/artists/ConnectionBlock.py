""" An artist to visualize a IceHive::Relation(Map) in steamshovel"""

from icecube.shovelart import *
from icecube import icetray, dataclasses
from matplotlib.colors import colorConverter as mplcol

from icecube import ToolZ, IceHiveZ


class ConnectionBlock( PyArtist ):
    numRequiredKeys = 1

    def isValidKey( self, frame, key_idx, key ):
        tn = frame.type_name(key)
        if key_idx == 0:
          if tn == "I3Geometry":
            if os.path.exists(self.setting("filename")):
              return True
            else:
              print ("require filename does not exist")
        return False


    def __init__(self):
        PyArtist.__init__(self)
        
        self.defineSettings((
            "colormap", I3TimeColorMap(),
            "scale", RangeSetting( 0.0, 100.0, 100, 10.0 ),
            "static", PyQColor.fromRgb( 255, 0, 255 ),
            "OMKeys", OMKeySet(),
            "custom color window", "",
            'filename', 'Text label',
            "conectorindex", RangeSetting( 0, 100, 100, 1 )
        ))
        
        self.ran_once = False


    def create( self, frame, output ):
      
        if not os.path.exists(self.setting"filename")):
          print ("require filename does not exist")
          return
      
        static_color = self.setting("static")
        keys = self.setting( 'OMKeys' )
        scale = self.setting('scale')
        geo = frame['I3Geometry']
          
        # protect against missed clicks
        keys = [k for k in keys if k in geo.omgeo]
        if not len(keys):
          return
        icetray.logging.log_info("Klick "+str(keys[0]))
        
        for o in self.getConnectedOMKeys(keys[0], frame):
          sphere = output.addSphere( scale, geo.omgeo[o].position )
          sphere.setSelectionContent( o )
          sphere.setColor( static_color )
        


    def getConnectedOMKeys(self, om, frame):
      geo = frame['I3Geometry']
      calib = frame["I3Calibration"] if frame.Has("I3Calibration") else None
      status = frame["I3DetectorStatus"] if frame.Has("I3DetectorStatus") else None
      if not self.ran_once:
        #get me a hive connection block
        self.cb = hcb.ConnectorBlock.from_file(self.setting("filename"))
        self.ran_once = True
      
      self.relationMap = self.cb.Get(self.setting("connectorIndex"))
      con_oms = []
      #om_pos = geo.omgeo[om].position
      for o in geo.omgeo.keys():
        #dist = om_pos-geo.omgeo[o].position
        #if dist.magnitude <= 300:
        #  con_oms.append(o)
        if self.relationMap.areRelated(om, o):
          con_oms.append(o)
          #pass
      return con_oms
    