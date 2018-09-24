The clustering-selector HiveClustering
===================================

Introduction
^^^^^^^^^^^^
I3HiveClustering is a module which performs a similar task of hit-cleaning as I3HiveCleaning, however, it applies the HiveSplitter algorithm to identify physics clusters on any PulseSeries. I3HiveClustering can be applied to any PulseSeriesMap in any frame and writes out all identified clusters, their union, aka the physics hits, and the noise hits.


I3IceClustering Module
^^^^^^^^^^^^^^^^^^^

The module ``I3HiveClustering`` applies the HiveSplitter algorithm to any PulseSeries. It is possible to run the algorithm either on Q-frames or P-frames by selecting the ``Stream`` parameter.

This is the full list of parameters to the module:

The Module IceHive takes the parameters:

* **Stream** [Default=Physics] The I3Frame-Stream to execute on.
* **InputName** [No Default] Name of the input PulsesSeriesMap.
* **OutputName** [No Default] Name the output PulseSeriesMap the processed pulses will be written to.
* **Multiplicity** [Default=3] Minimum number of mutually connected DOMs in order to form a cluster; In the further processing: Required multiplicity **minus one** of connected DOMs in each cluster to any further hit, in ordered that the hit is assigned to that cluster (the hit counts as one itself).
* **MultiplicityTimeWindow** [Default=1000 ns] Time span within which the multiplicity requirement must be met.
* **AcceptTimeWindow** [Default=NAN] Connect all hits on same DOM up to this time limit after the initial hit regardlessly [ns]; deactivate by 'NAN'
* **RejectTimeWindow** [Default=INF] Reject all hits on same DOM from to this time limit after the initial hit regardlessly [ns]; deactivate by 'INF'
* **MergeOverlap** [Default=1] Number of overlapping DOMs required for (partial) subevents to be merged
* **ConfigurationBlock** [No Default] A block of ConfigurationServices for HiveSplitter (specify either this or ConnectionBlock)
* **ConnectorBlock** [No Default] A block of preconfigured ConnectionServices (specify either this or HiveConfigurationBlock)
* **If** [Default=lambda f: True] An optional function for conditional execution.