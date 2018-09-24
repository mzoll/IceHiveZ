The project IceHive
=========================


Overview
^^^^^^^^

The project contains the two major modules:
* **HiveSplitter**, an advanced event splitter
* **HiveCleaning**, a hit-cleaning removing isolated pulses

It is supported by the universal libraries:
* **OMKeyHash**, a way to compactly hash OMKeys and construct fast acces look-up tables
* **HitSorting**, a way to cache PulseMaps into ordered series of compact objects
* **Hive-lib**, describes the detector geometry by vertical strings in realtion to each other
* **IndexMatrix**, provides symmetric and asymmetric matrices, which can hold any type of information as values
* **DistanceService**, in combination with OMKeyHash facilitates the repeated computation of distances between DOMs

The general term hit
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The algorithm is designed to run on data that is described by hits, which are fully defined by a four vector of position and time. Because in the IceCube detector the size of DOM,  measuring instrument, is small against the distance between DOMs and each DOM has a fixed position within the detector, as well as each DOM has a unique identifier, a hit might be fully described by the pair of an OMKey(hash) and the time of any hit DOM.

Because in icetray application a hit is usually represented by more complex types which hold much more information (I3RecoPulse, I3DOMLaunch etc.) the internal representation, the hit, holds essential information only. So even though the algorithm takes such complex I3-types as input and output, the hit is the base object that the algorithm in fact works with.

There are two main classes of hits:
* ``Physics hits`` caused by a particle event in the detector and the photons received at the DOM.
* ``Noise hits`` are the opposite of Physics hits, caused by thermal emission, radioactive decays etc. in the DOM, an detector intrinisic intrinsic property


General use-case
^^^^^^^^^^^^^^^

The project has a general use-case for any incident that one desires to perform lowest level data-treatment on RecoPulses. This especitally includes the application in base-processing at the filter level.
The event splitter module HiveSplitter is able to select from a series hits these which are clustering, a property of hits which are caused by a particle event, the interesting physics hits. Thereby it is able to separate Physics from Noise hits. Simulataniously this implements the splitting of coincident events by the same algorithm, as they will manifest as separate clusters of hits. This algorithm performs in general fast and dependently even in high noise environments.

The algorithm linearly works off hits, which are supplied as a time-ordered stream of named hits. Thereby the execution time of the algoirthm scales linearly with the number of hits. By this method even the processing of **very** long events is possible, where iterative approaches easily break down, when the number of hits becomes huge.

HiveCleaning provides additional cleaning options in a forwards iterative approach for further outlier (noise) removal, hard to treat in the native HiveSplitter algorithm. The so treated data may, a clean selection of hits in each particle subevent, may then be used for event reconstructions.


Hits vs. Pulses
^^^^^^^^^^^^^^^

Within this project the terms ``Pulse`` and ``Hit`` will be used quite often and in a not very differentiated way. In principle both terms refer to the ocurence of a physical response in the instrument after a certain physical incident. In IceCube where we have the DOMs as instruments this holds true and the physical incident can always be rooted back to a photon hitting the photo-cathode, liberating a photo-electron, generating an electron avalanche, registration photo-current, its digitization and finally pulse-template evaluation. So the terms Hits and Pulse just refer to the stage at which the corresponding physical response is processed at. However, both still stand for the same concept and within this common concept they are used in the context of this project.
Thereby a Hit or Pulse is just the information of a certain *DOM* processing a signal with arbitrary characteristics at a certain *time*.

For practical use in the context of icetray a *pulse* may refer to an *I3RecoPulse*, which is the abstraction format at which the analysis software picks up the processing. A *hit* may than be the *minimal representation of a pulse* as internally processed in an algorithm.

Furthermore the term ``PulseSeries`` should refer to any ordered series of Pulses in arbitrary format;
and the term ``PulseSeriesMap`` refers to the  storage representation in I3-format, which is a ordered map of DOMs and the ordered series of Pulses which were registered at them.



Driving principle of HiveSplitter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

HiveSplitter works by forming clusters of hits, which are mutually connected. Working through the series of hits the algorithm so forms parallel hit-clusters which all are individually established, evolving and concluding.
Hit-clusters bigger than a certain multiplicity requirement on the number of hits they contain are identified as a subevent and are written out as such.
Hits attributed to such a cluster are considered to be the physics hits caused by an particle event, while all other hits are regarded as noise.
Keeping individual developing hit-clusters separated in the detector allows for the identification of multiple parallel subevents at once, and forms the basis for the splitting capability of the algorithm.


Clusters are formed by hits connecting to each other and fulfilling the multiplicity requirement on the number of DOMs each hit can connect to at each point in time. The type of connections a hit can make to other hits are fueled by arguments, which express physical causality of photons registered to origin from the same particle event. Many different such arguments, called *(Hit)Connector*, can be found, so for example:
* Hits are caused by photons emitted in the same emission topology, Cherenkov-cone or cascade, in a narrow time window
* Hits are caused by photons of the same emission topology, e.g. the Cherenkov-cone, which has propagated through the detector
* Hits are caused by photons which are riding on a common light-emission front, while it has further propagated.
For most purposes the exact relation between two hits identified as connected is uninteresting, and so a list of possible Connectors is usually evaluated by an end result taking the **OR** of all individual results.

Connectors are required to fulfill two critical properties: they need to be limited in time and space. This meaning the time and space distance for an earlier hit to connect to a removed, later hit needs to be absolutely limited in the space-time parameter-space.

Doing so allows us for each hit to define a volume in space-time, where it can connect to further hits, called the *hit's active volume (IAV)*. This volume is active as long as the generating hit can still connect to other hits, and expires after a certain time. Therefore in the progression of time hits stay active only for a certain time after their ocurence. 
Each cluster, thus a collection of hits, has an overlapping volume of IAV from all its substituting hits, the *summed active Volume (SAV)*.
Certain regions within the SAV have an overlap in IAV which exceeds a certain, externally superimposed multiplicity requirement, and are called the *cluster's active Volume (CAV)*. The algorithm will only collect a new hit into the same cluster if it falls in this CAV, so fulfills the multiplicity requirement of connections it can make to previous hits substituting the cluster. With each hit that is added to the cluster, the SAV is expanded by the additional IAV of that hit and the CAV is altered. Because the IAV of each individual hits is time-dependent and has a finite lifetime, the CAV continuously evolves with the progression of time. This also means that past hits will continuously expire, exerting no IAV anymore, and so removing possible overlap from the SAV and shrinking the CAV. This describes a very dynamical process for the inclusion probability of any future hit to an cluster depending on the current state of its CAV, where the chance for inclusion is highest if the hit ocures possibly close to other hits in space and time; the clustering requirement.

Because numberous hits are created through a traversing particle in the detector on the Cherenkov-cone, the CAV will follow the path of further created light and hits, and thus the particle itself as long as the particle create enough hits to nourish the clusters evolution. Cascade emissions are likewise included in this description with new hits being generated on the spherical expanding light front.


Connector Substructure
^^^^^^^^^^^^^^^^^^^^^^
In the implementation for HiveSplitter a Connector in its purpose to evaluate the connectivity of two hits is fully described by its two sub-components:
* **Relation**: a causal relation in functional form that takes the parameters of the absolute time and absolute space distance between the two hits
* **Connection**: a list-like map if the first hit DOM is to be considered to make the named causal relation to any other DOM and which DOMs so are eligible.

This implementation allows the fast evaluation of the connection between any two hits. As the evaluation on the Connection can be implemented in a look-up fashion and is unique, which makes this fast, the evaluation of the Relation is only executed on a need to basis and can so be of high computational complexity.


C++ templates
^^^^^^^^^^

IceHive uses templates in its implemented C++ libraries so that they can be used with many different types of pulses. The conceptually most central type are ``I3RecoPulses``. Most library functionality is implemented for the usual high-level icetray objects of the signature ``I3Map<OMKey,vector<Response>>``, so for example the ``I3RecoPulseSeriesMap`` or in short *RecoMap*.

The algorithmic implementations are written for general hit-objects which can be extracted or obtained for many different pulse types.
