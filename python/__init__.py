from icecube.load_pybindings import load_pybindings
load_pybindings(__name__,__path__)
del load_pybindings


#try:
  #from icecube.shovelart import Scenario
  #from artists import ExplodingSpheres
  #Scenario.registerArtist(ExplodingSpheres)
  #from artists import ConnectionBlock  
  #Scenario.registerArtist(ConnectionBlock)
#except:
  #pass
  
from icecube.IceHiveZ.pydefs import DefaultCB_Splitter, DefaultCB_Clean
