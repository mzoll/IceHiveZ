Configuring the algorithm
==========================

All modules of IceHive run based on the evaluation of a list of Connectors which express possible types of connections a hit in the detector can make. This collection of Connectors that is hold internally is called a *ConnectorBlock*. This ConnectorBlock is created, or here called *built*, and stored by the algorithm internally once enough information has been collected to fully define it. A blueprint definition is specified by the so-called *ConfigurationBlock*, which is convoluted with Detector geometry information, typically from the GCD-objects, to build the ConnectorBlock. This ConfigurationBlock contains information which types of causal Connection are to be evaluated and which DOMs can partake. In contrast to let the algorithm internally hold and create the ConnectorBlock, it is possible to externally pass a ConnectorBlock directly to the algorithm. This allows much exacter steering, as the ConnectorBlock can be defined in greater detail by external editing beyond the capabilities of the internal configuration process. 


ConfigurationBlock
^^^^^^^^^^^^^^^^^^^^^^

A ConfigurationBlock is a plain list of *Configurations*, each Configuration in turn is the specification how a Connector is to be build up, which DOMs it incorporates and which causal Connection is to be considered. These two properties are specified by the *ConnectionConfig* and *RelationConfig* respectively. 

While for the ConnectionConfig the choice of options is limited, the RelationConfig is more complex and requires more consideration. The most simplest type for a RelationConfig is the *SimpleRelationConfig*, where a user-specified function defines which DOMs are to be considered related. A more complex choice and central part of this project is the *HiveRelationConfig*. It encodes these relations by specifying which type of DOMs (*OMType*) should connect to which other type of DOMs, and how far around each considered DOM a possible relation might extant (*RingLimits*).

This complex structure allows to build many Connectors in parallel of which each is specifically optimized on a different sub-region in the detector; for example IceCube and DeepCore.

HiveRelationConfig
^^^^^^^^^^^^^^^^^^^
As this will be the predominantly used type of RelationConfig lets take a closer look how to steer it.


Specifying RingLimits
---------------------

Here the inclusion volume in the comparison of two hits is defined.

  ..image::rings.png
  (Regular single dense strings in green, double dense strings in red, and triple dense strings in blue; compare to the surface assembly of IC86 detector)

The parameter is a list of the vertical distance intervals relative to current z-ordinate in which DOMs are included. Each interval specified counts out from a central string to an increasing numbers of rungs of strings which wrap around this central string *C*. Normally these rings and strings that they contain are well defined in the nominal IC86 geometry, so for example considering central string 36 this would yield: ::

  Ring0 = (36) == C
  Ring1 = (26,27,35,37,45,46),
  Ring2 = (17,18,19,25,28,34,38,44,47,54,55,56) etc.

each entry of intervals specified as left and right value limit can take the following values:
* ``(double)`` = connect that DOMs in this vertical distance on this ring,
* ``NAN`` = no connection in this direction,
* ``INF`` = connect everything in this direction.
The internal comparison that is made has the form::
  
  limit_down <= z_dist <= limit_up
  
where ::z_dist:: is the vertical distance of the between the DOMs with ::z_C:: being a central fixed DOM and ::z_i:: is the DOM evaluated, defined as::
  
  z_dist = z_i - z_C

The RingLimits thereby have the form::
  
  [(Ring0_Down, Ring0_Up), (Ring1_Down, Ring1_Up), ... ]

and can only be configured in pairs of (up, down) for each ring. For example a configuration of::
  
  ICRingLimits=[(-10,10), (-15, 5), (NAN, INF)]

does imply for an early Hit (E) on OMKey(36,30) that the following OMKeys are eligible to find neighbouring hits (L) (furthermore assume for now that DOMs are spaced 1m apart):
::

  {(String36, {20-40}), ({Ring1}, {15-35}), ({Ring2}, {30-60}); 

If a DOM itself should be considered connected to itself can be steered by the parameter *SelfConnect*.


The different designation SingleDense, DoubleDense and TripleDense refer to the regular different hexagonal geometries.
* SingleDense refers to the IceCube geometry (characteristic spacing 125 meter).
* DoubleDense refers to a regular geometry added one more strings to every triangular cell, aka one further string in between any three other strings characteristic spacing 72 meter)
* TripleDense refers to a regular geometry with four more strings added to every triangular cell, aka 2 strings on each arm of the triangular cell and one in the middle

  ..image::geo.png
  (Regular single dense strings in green, double dense strings in red, and triple dense strings in blue; compare to the surface assembly of IC86 detector)


ConnectFrom and ConnectTo
^^^^^^^^^^^^^^^^^^^^^^^^^
These parameters signal which DOMs are eligible to connect. As the connections are explicitly not bi-directional there are different parameters for the DOMs that can establish a connection, normally by being hit first, and which can receive connections, normally being hit later.

The specification of DOMs happens by parsing a python object to this parameter that can be called with a single argument, which is an OMKey and evaluates as ``True`` or ``False``.

Function
----------
The simple function call of the following form might already suffice to include all IceIce DOMs::

  def IsIceIceDom(omkey):
    return (omkey.string<=86 and omkey.om<=60)

Callable object
---------------------
Alternatively one can pass a python object that provides a call operator with a single argument, beeing the omkey::

  class IsIceCubeDom():
    def __init__(self, i3geo):
      self.i3geo = i3geo
    def __call__(self, omkey):
      return self.i3geo.omgeo[omkey].omtype == dataclasses.I3OMGeo.IceCube
    
OMTopology
------------
Alternatively a set of DOMs which are contained in a predefined set, for example by them being in specify topological or geometrical configuration like DeepCore or IceTop. For this it suffices to load a predfined Detector OMTopology definition and to specify the OMTopologies that should be considered. ::

  IceHiveZ.OMTopology(["IceCube"]), IceHiveZ.IC86_OMTopology())


Connections
^^^^^^^^^^^^
The Connections which are evaluted need to be configured and specified at this stage. 

    
ConnectorBlock
^^^^^^^^^^^^^^^^^
A ConnectorBlock is what is created from the information in the (Hive)ConfigurationBlock. It can exist and be created also as an independent object by definition through the user. However, it is in general advisable to use the HiveConfiguationBlock to create it by the later discussed Manually Building function and edit it later. The Connection Block is always created by using information external to the ConfigurationBlock, like the detector information in the GCD-objects, which is why it holds pointers to these objects internally.

The structure of the ConnectorBlock is similar to that of the ConfigurationBlock it was created from. A ConnectorBlock is a list of Connectors each corresponding to a HiveConfiguration. Each Connector contains the same Connections and a RelationMap, which is a bi-indexed map of all DOMs holding all possible connections as a lookup-table. For efficiency these tables index all DOMs by a *OMKeyHasher* that is always hold internally. There is the possibility for direct access to these DOMmaps by the same hasher values, which is advantageous as is bypasses the need for internal conversion processes forth and back from ``OMKey`` to ``OMKeyHash``.

The ConnectorBlock also holds internal helper services as for example the *DistanceService* facilitating the retrievals of DOM positions and distances from the I3Geometry.


Building the ConnectorBlock manually
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Once a ConfigurationBlock has been defined and GCD information is accessible, the ConnectorBlock can be created. While is normally done internally in the respective ``I3Modules`` this configuration can also be done by hand in the python interface. Thereby one obtains a ConnectorBlock and associated objects like they are held algorithm internally. These objects can than be inspected and edited for further customization, where the blue-print configuration through HiveConfiguration is too crude. Also as most of the used information for obtaining the ConnectorClock remains static, it can be advanatageous to create the ConnectorBlock only once, hold it external and pass itself as a static object to every instance of IceHiveZ's ``I3Modules``.

In order to obtain the ConnectorBlock the central objects of the GCD, the ``Ì3OMGeoMap` need to be passed to the BuildConnectorBlock function. A ConnectionBlock created like this is a self-contained unit, so it can be stored and recalled at any point.


(Hive)ConfigurationBlock vs ConnectorBlock
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
One of the biggest advantages IceHiveZ is that it can save and load a pre-generated ConnectorBlock, generated either by hand or pre-configured by a (Hive)ConfiguationBlock and a GCD file. This is advantages as it is a very fast process compared to its (repeated) configuration during run-time, which can be very time-consuming and can hold up further processing. Also the fact that for the processing of whole years the nominal GCD-files are static in the most central objects like the ``Ì3Geometry`` makes this a reasonable choice. It is highly recommended to use this feature to its fullest, as this offers great flexibility and possibilities for detailed customized ConnectorBlocks specific to certain analysis tasks.

Please make use of this great feature, but beware to check if the loaded ConnectorBlock is in its configuration suitable for the data that you are processing!
