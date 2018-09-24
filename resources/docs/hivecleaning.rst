HiveCleaning
============

Introduction
^^^^^^^^^^^^

HiveCleaning provides a classic Hit-cleaning that is based on the hexagonal description of the detector as it is used in the HiveSplitter. This enables a more precise steering of the parameters and a more powerful cleaning process.

The employed algorithm is similar to the classic RT-cleaning: Isolated hits, which do not have neighbouring hits occurring within a certain distance within a certain time window, are removed. Because Noise-hits predominantly occur randomly and show as isolated hits in the detector array, while Physics-hits occur in a clustering fashion, this algorithm is excellently suited for noise-cleaning.

I3HiveCleaning Module
^^^^^^^^^^^^^^^^^^^^^

The module ``I3HiveCleaning`` applies the HiveCleaning algorithm to a PulseSeries. It is possible to run the algorithm either on  different frames types by selecting the ``Stream``-parameter.

This is the full list of parameters to the module:

* **Stream** [Default=Physics] The I3Frame-Stream to execute on.
* **InputName** [No Default] Name of the input PulsesSeriesMap.
* **OutputName** [No Default] Name the output PulseSeriesMap the processed pulses will be written to.
* **Multiplicity** [Default=1] Required multiplicity of connected neighbours for each hit, in order to remain in the PulseSeries.
* **MaxTimeResidualEarly** [Default=-INF] Maximum positive time which will allow a close-by pair of hits to be considered connected in [ns]
* **MaxTimeResidualLate** [Default=INF] Maximum positive time which will allow a close-by pair of hits to be considered connected in [ns]
* **HiveConfigurationBlock** [No Default] A block of HiveConfigurationServices for HiveSplitter (specify either this or ConnectionBlock)
* **ConnectionBlock** [No Default] A block of preconfigured ConnectionServices (specify either this or HiveConfigurationBlock)
* **If** [Default=lambda f: True] An optional function for conditional execution.

The module delivers output as an PulseSeriesMapMask in under the key ``OutputName`` in the target frame.


Best Practices
^^^^^^^^^^^^^^

Despite the fact that this module has been developed for in the hindsight of noise-cleaning, it is also very well suited for the removal of confusing hits, which might be problematic for reconstructions.

These might include, but are not limited to:
* Early/Late Hits which are in fact Physics-hits, but distort the start/end times of events.
* Hits which are in fact Physics, but occur at the border of the Cherenkov-cone. These originate from long-propagated survivor photons, which have scattered multiple times. Thereby they fuzz-out the otherwise sharp characteristics of the Cherenkov cone. Their removal and reduction of the PulseSeries to more direct hits, the bright central core, can significantly improve reconstruction results.
* Afterpulses, which are instrumentation artefacts, occur long after the causal (Physics) from a through-going particle has long since concluded.
* Isolated hits of great pull to reconstruction. Some reconstructions are very vulnerable if the PulseSeries presented to them is not clean from any artefacts, which are not well described by the modelled event topology. These can introduce a significant pull to the computed track-hypothesis and should be removed beforehand.
