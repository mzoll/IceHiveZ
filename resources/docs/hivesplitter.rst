The event splitter HiveSplitter
==========================

Introduction
^^^^^^^^^^^^
HiveSplitter is the central heard of this project. HiveSplitter, when used correctly, is a very powerful tool in IceCube data processing at lowest levels with little or no treatment of the data by hit-cleaning. In fact the performance of HiveSplitter is better the more complete the original PulseSeries remains at the execution of HiveSplitter. This, paired with its low noise vulnerability, makes HiveSplitter applicable for earliest event splitting and cleaning.


I3HiveSplitter Module
^^^^^^^^^^^^^^^^

The module ``I3HiveSplitter`` applies the HiveSplitter algorithm to any PulseSeries found in the ``Q``-frame. Output are a series of subevent ``P``-frames.

This is the full list of parameters to the module, whch are later explained in more detail:

The Module IceHive takes the parameters:

* **SubEventStream** [No Default] Name of the SubEventStream the P-frames are written out to.
* **InputName** [No Default] Name of the input PulsesSeriesMap.
* **OutputName** [No Default] Name the output PulseSeriesMap the processed pulses will be written to.
* **SaveSplitCount** [Default=False] Save an integer in the frame indicating the number of subevents generated
* **Multiplicity** [Default=3] Minimum number of mutually connected DOMs in order to form a cluster; In the further processing: Required multiplicity **minus one** of connected DOMs in each cluster to any further hit, in ordered that the hit is assigned to that cluster (the hit counts as one itself).
* **MultiplicityTimeWindow** [Default=1000 ns] Time span within which the multiplicity requirement must be met.
* **ReadoutWindowMinus** [Default=4000 ns] Length of readout window to pad before trigger [ns]. This time is used in the TriggerSplitter pulse extraction and is added to the '_Noised' pulse output for every subevent.
* **ReadoutWindowPlus** [Default=6000 ns] Length of readout window to pad after end of trigger [ns]. This time is used in the TriggerSplitter pulse extraction and is added to the '_Noised' pulse output for every subevent.
* **TrigHierName** [Default="I3TriggerHierarchy"] Name of the input TriggerHierarchy
* **UpdateTriggerHierarchy** [Default=True] Should the TriggerHierarchy by updated
* **AcceptTimeWindow** [Default=NAN] Connect all hits on same DOM up to this time limit after the initial hit regardlessly [ns]; deactivate by 'NAN'
* **RejectTimeWindow** [Default=INF] Reject all hits on same DOM from to this time limit after the initial hit regardlessly [ns]; deactivate by 'INF'
* **MergeOverlap** [Default=1] Number of overlapping DOMs required for (partial) subevents to be merged
* **ConfigurationBlock** [No Default] A block of ConfigurationServices for HiveSplitter (specify either this or ConnectorBlock)
* **ConnectorBlock** [No Default] A block of preconfigured ConnectionServices (specify either this or ConfigurationBlock)

You can only configure either **ConfigurationBlock** or **ConnectionBock**, as they mutually exclude each other. If a ConfigurationBlock is configured, the algorithm will reconfigure the internally created ConnectorBlock every time any detector configuration is passes by in the frame-stream (GCD-frame). If a predefined ConnectorBlock is specified, on the passing by of a new detector configuration only the compatibility of the new objects is checked. It is the responsibility of the analyzer to provide a compatible, pre-configured ConnectorBlock befitting for the data that is read.


Output
^^^^^^

The output of IceHive according to the selection of parameters are following objects:

in the Q-frame:
* <I3RecoPulseSeriesMapMask>([OutName]_Cluster[00]) any identified cluster indexing with two integers
* <I3RecoPulseSeriesMapMask>([OutName]_Physics) containing ALL found clusters
* <I3RecoPulseSeriesMapMask>([OutName]_Noise) the inverse of [OutName]_Physics taken in respect to [InputName]
* <I3Int>([SubEventStream]_SplitCount) The number of found clusters

in the P-frame in [SubEventStream]: (one for each found cluster in time-order fashion)
* <I3RecoPulseSeriesMapMask>([OutName]) containing the pulses of the cluster
* <I3TimeWindow>([OutName]_TimeRange) containing the start and end-time of the cluster
* <I3RecoPulseSeriesMapMask>([OutName]_Noised) containing the cluster and the noise around the specified time-window
* <I3EventHeader>(I3EventHeader) correct with the timing information of the start and end-time of the cluster, eventually plus the trigger-readout windows
* <I3TriggerHierarchy>(I3TriggerHierarchy) the updated Trigger Hierarchy to the requested trigger-windows and selections


How to optimize
^^^^^^^^^^^^^^^

**There is always room for improvement.**

The default parameters in this project are not optimized. They are first guess values fuelled by experience from previous approaches to event splitting or hit-recognition/cleaning. However they work quite well and the performance with default settings is more than satisfactory.

However these parameters might want to be optimized in the future to strengthen the splitting capability or increase the selectiveness on Physics-hits. Here are some pointers how to approach this:
* First, you will need to do your quantification on MC data-sets and compare them to data as a verification. In these MC data-sets you need to have the hit truth present for those objects you plan to operate on. For I3RecoPulses you can use the projects 'MCHitSeparator' or 'MCPulseSeparator' to recover this information from simulation.
* For any change, you need to observe two properties very closely: The splitting capability vs the number of wrong splits, and the physics-purity vs noise-contamination in the output. There are tools in MCPulseSeparator that can help you with this.
* Optimize eligibility volume first: its tempting to manipulate the simple time-settings first, however you might over-optimize this faster than  the eligibility volume.
* Geometrical analysis of your proposed settings in the topology of the detector can get you a long way: for example observe blind paths, track-length between DOMs for certain angles
* Currently the inclusion volumes are symmetrical up and down on each string, however the DOM sensitivity is much higher for up-going particle movement. This means that in principle fewer DOMs have to be included upwards on the rings than down-wards.
* Before removing whole rings, think about the blind paths and probability of any track still making enough hits so that the driving principle is not impaired.
* Do not over-optimize the time-settings to their respective purpose: It has shown that somewhat looser settings can help the algorithm with deficiencies, which arise from irregularities in the geometrical setup, the detector-response and even physical properties.
* Be more easy on the positive residual boundary (arriving to late) for photon propagation than on the negative one (arriving too early): This has to do that this parameter considers direct photon-hits. However we know there is scattering in the ice, which delay photons and increase their time-residual. this is also an effect which increases over travelled distance. The future might bring that such model can be better integrated into the algorithm.
* It is highly discouraged to experiment with settings which are infinite (INF) or not-a-number (NAN). Such can break the driving principle and cause other bad things to happen.
