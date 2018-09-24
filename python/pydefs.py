"""
defines default objects for IceHiveZ
"""
from icecube.icetray import I3Units
from icecube.dataclasses import I3Constants

#==================================================
def DefaultCB_Splitter(add_DC=True):
  import os
  from icecube import ToolZ, IceHiveZ
  
  #make the omt
  #omt_IC86 = IceHIveZ.OMTopology.from_file(os.path.join(os.path.expandvars("$I3_BUILD"), "IceHiveZ/resources/data/IC86_domTopo.dat"))
  omt_IC86 = ToolZ.IC86Topology.OMTopologyMap() #use a preconfigured DOMTopo Map for now for IC86
  
  #create an ad hoc empty HiveConfigurationBlock, that we will fill with Connectors
  hcb = IceHiveZ.ConfiguratorBlock()
  
  #only treat in ice DOMs; this ignores for example IceTop DOMs
  hcb.takeOMKeys(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["InIce"]),omt_IC86))
  
  #generate the string topology in the detector
  ht_IC86_ICarray = IceHiveZ.HiveTopology.from_config(os.path.join(os.path.expandvars("$I3_BUILD"), "IceHiveZ/resources/data/SingleDenseStars.dat"))

  #<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  #create a new configurator
  cs_IC_particle = IceHiveZ.Configurator("IC_particle")
  
  #configure the connector: here dynamic connector with light-speed
  con_IC_particle = IceHiveZ.DynamicConnectionConfig()
  con_IC_particle.speed = I3Constants.c
  con_IC_particle.time_residual_early = 50. #ns
  con_IC_particle.time_residual_late = 200. #ns
  cs_IC_particle.AddConnectionConfig(con_IC_particle)
  
  #configure the considered DOMs: with the connector, limit-pairs, and the hive configuration file
  rel_IC_particle = IceHiveZ.HiveRelationConfig(ht_IC86_ICarray)
  rel_IC_particle.AddRingLimits([(-90., 90.),(-272.7, 272.7),(-417., 417.)])
  rel_IC_particle.selfConnect = False
  rel_IC_particle.mutuallyConnect = False
  rel_IC_particle.AddConnectFrom(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["IceCube"]) ,omt_IC86))
  rel_IC_particle.AddConnectTo(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["IceCube"]),omt_IC86))
  cs_IC_particle.AddRelationConfig(rel_IC_particle)
  
  #add the configurator to the configuration block
  hcb.add(cs_IC_particle)
  #<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  cs_IC_photon = IceHiveZ.Configurator("IC_photon")
  
  con_IC_photon = IceHiveZ.PhotonDiffusionConnectionConfig()
  con_IC_photon.time_residual_early = 50. #ns //early
  con_IC_photon.time_residual_late = 50. #ns //late 
  con_IC_photon.lower_cont_quantile = 0.05
  con_IC_photon.upper_cont_quantile = 0.9
  con_IC_photon.min_pdfvalue = 0.
  cs_IC_photon.AddConnectionConfig(con_IC_photon)
  
  rel_IC_photon = IceHiveZ.HiveRelationConfig(ht_IC86_ICarray)
  rel_IC_photon.AddRingLimits( [(-90., 90.),(-272.7, 272.7)] )
  rel_IC_photon.selfConnect = False
  rel_IC_photon.mutuallyConnect = False
  rel_IC_photon.AddConnectFrom(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["IceCube"]),omt_IC86))
  rel_IC_photon.AddConnectTo(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["IceCube"]),omt_IC86))
  cs_IC_photon.AddRelationConfig(rel_IC_photon)
  
  hcb.add(cs_IC_photon)
  #<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  cs_IC_static = IceHiveZ.Configurator("IC_static")
  
  con_IC_static = IceHiveZ.DeltaTimeConnectionConfig()
  con_IC_static.time_difference_early = 200. #ns
  con_IC_static.time_difference_late = 200. #ns
  cs_IC_static.AddConnectionConfig(con_IC_static)

  rel_IC_static = IceHiveZ.HiveRelationConfig(ht_IC86_ICarray)
  rel_IC_static.AddRingLimits( [(-100., 100.),(-100., 100.),] )
  rel_IC_static.selfConnect = True
  rel_IC_static.mutuallyConnect = False
  rel_IC_static.AddConnectFrom(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["IceCube"]),omt_IC86))
  rel_IC_static.AddConnectTo(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["IceCube"]),omt_IC86))
  cs_IC_static.AddRelationConfig(rel_IC_static)
  
  hcb.add(cs_IC_static)  
  #<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>
    
  ht_IC86_DCarray = IceHiveZ.HiveTopology.from_config(os.path.join(os.path.expandvars("$I3_BUILD"), "IceHiveZ/resources/data/DoubleDenseMod.dat"))
  
  #configure the connector: here dynamic connector with light-speed
  cs_DC_particle = IceHiveZ.Configurator("DC_particle")
  
  con_DC_particle = IceHiveZ.DynamicConnectionConfig()
  con_DC_particle.speed = I3Constants.c
  con_DC_particle.time_residual_early = 100. #ns
  con_DC_particle.time_residual_late = 100. #ns
  cs_DC_particle.AddConnectionConfig(con_DC_particle)

  rel_DC_particle = IceHiveZ.HiveRelationConfig(ht_IC86_DCarray)
  rel_DC_particle.AddRingLimits([(-70., 70.),(-131.5, 131.5),(-250.8, 250.8)])
  rel_DC_particle.self_connect = False
  rel_DC_particle.mutuallyConnect = False
  rel_DC_particle.AddConnectFrom(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["DeepCoreFidutial","DeepCoreCap"]),omt_IC86))
  rel_DC_particle.AddConnectTo(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["DeepCoreFidutial","DeepCoreCap"]),omt_IC86))
  cs_DC_particle.AddRelationConfig(rel_DC_particle)
  
  hcb.add(cs_DC_particle)
  #<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>
  
  cs_DC_photon = IceHiveZ.Configurator("DC_photon")

  con_DC_photon = IceHiveZ.PhotonDiffusionConnectionConfig()
  con_DC_photon.time_residual_early = 50. #ns //early
  con_DC_photon.time_residual_late = 50. #ns //late 
  con_DC_photon.lower_cont_quantile = 0.05
  con_DC_photon.upper_cont_quantile = 0.9
  con_DC_photon.min_pdfvalue = 0.
  cs_DC_photon.AddConnectionConfig(con_DC_photon)

  rel_DC_photon = IceHiveZ.HiveRelationConfig(ht_IC86_DCarray)
  rel_DC_photon.AddRingLimits([(-70., 70.),(-131.5, 131.5),(-250.8, 250.8)])
  rel_DC_photon.self_connect = False
  rel_DC_photon.mutuallyConnect = False
  rel_DC_photon.AddConnectFrom(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["DeepCoreFidutial","DeepCoreCap"]),omt_IC86))
  rel_DC_photon.AddConnectTo(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["DeepCoreFidutial","DeepCoreCap"]),omt_IC86))
  cs_DC_photon.AddRelationConfig(rel_DC_photon)
  
  hcb.add(cs_DC_photon)
  #<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  
  cs_DC_static = IceHiveZ.Configurator("DC_static")

  con_DC_static = IceHiveZ.DeltaTimeConnectionConfig()
  con_DC_static.time_difference_early = 150. #ns
  con_DC_static.time_difference_late = 150. #ns
  cs_DC_static.AddConnectionConfig(con_DC_static)

  rel_DC_static = IceHiveZ.HiveRelationConfig(ht_IC86_DCarray)
  rel_DC_static.AddRingLimits([(-100., 100.),(-100., 100.)])
  rel_DC_static.self_connect = True
  rel_DC_static.mutuallyConnect = False
  rel_DC_static.AddConnectFrom(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["DeepCoreFidutial","DeepCoreCap"]),omt_IC86))
  rel_DC_static.AddConnectTo(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["DeepCoreFidutial","DeepCoreCap"]),omt_IC86))
  cs_DC_static.AddRelationConfig(rel_DC_static)

  hcb.add(cs_DC_static)
  
  #<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>
  return hcb


#==================================================
def DefaultCB_Clean(add_DC=True):
  import os
  from icecube import IceHiveZ
  
  #make the omt
  #omt_IC86 = IceHIveZ.OMTopology.from_file(os.path.join(os.path.expandvars("$I3_BUILD"), "IceHiveZ/resources/data/IC86_domTopo.dat"))
  omt_IC86 = IceHiveZ.IC86_OMTopology() #use a preconfigured DOMTopo Map for now for IC86
  
  ht_IC86_ICarray = IceHiveZ.HiveTopology.from_config(os.path.join(os.path.expandvars("$I3_BUILD"), "IceHiveZ/resources/data/SingleDenseStars.dat"))
  
  hcb = IceHiveZ.ConfiguratorBlock()

  cs_IC_dt = IceHiveZ.Configurator("IC_dt")

  con_IC_dt = IceHiveZ.DeltaTimeConnectionConfig()
  con_IC_dt.time_difference_early = 600. #ns
  con_IC_dt.time_difference_late = 600. #ns
  cs_IC_dt.AddConnectionConfig(con_IC_dt)
  
  rel_IC_dt = IceHiveZ.HiveRelationConfig(ht_IC86_ICarray)
  rel_IC_dt.AddRingLimits( [(-70., 70.),(-70., 70.),] )
  rel_IC_dt.selfConnect = True
  rel_IC_dt.mutuallyConnect = False
  rel_IC_dt.AddConnectFrom(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["IceCube"]),omt_IC86))
  rel_IC_dt.AddConnectTo(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["IceCube"]),omt_IC86))
  cs_IC_dt.AddRelationConfig(rel_IC_dt)

  hcb.add(cs_IC_dt)  

  ht_IC86_DCarray = IceHiveZ.HiveTopology.from_config(os.path.join(os.path.expandvars("$I3_BUILD"), "IceHiveZ/resources/data/DoubleDenseMod.dat"))
  
  cs_DC_dt = IceHiveZ.Configurator("DC_dt")
  
  con_DC_dt = IceHiveZ.DeltaTimeConnectionConfig()
  con_DC_dt.time_difference_early = 600. #ns
  con_DC_dt.time_difference_late = 600. #ns
  cs_DC_dt.AddConnectionConfig(con_DC_dt)
  
  rel_DC_dt = IceHiveZ.HiveRelationConfig(ht_IC86_DCarray)
  rel_DC_dt.AddRingLimits( [(-70., 70.),(-70., 70.),] )
  rel_DC_dt.self_connect = True
  rel_DC_dt.mutuallyConnect = False
  rel_DC_dt.AddConnectFrom(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["DeepCoreFidutial","DeepCoreCap"]),omt_IC86))
  rel_DC_dt.AddConnectTo(ToolZ.OMTopology_Comparator(ToolZ.OMTopology(["DeepCoreFidutial","DeepCoreCap"]),omt_IC86))
  cs_DC_dt.AddRelationConfig(rel_DC_dt)

  hcb.add(cs_DC_dt)
  #[(-70., 70.),(-144.1, 144.1),(-124.7, 124.7),(-82.8, 82.8)] for infill-string 79,80

  return hcb
