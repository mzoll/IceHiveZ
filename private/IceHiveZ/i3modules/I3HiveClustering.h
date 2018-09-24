/**
 * \file I3HiveClustering.h
 *
 * (c) 2012 the IceCube Collaboration
 *
 * $Id: I3HiveClustering.h 99900 2013-02-26 10:10:43Z mzoll $
 * \version $Revision: 99900 $
 * \date $Date: 2013-02-26 11:10:43 +0100 (Tue, 26 Feb 2013) $
 * \author Marcel Zoll <marcel.zoll@fysik.su.se>
 *
 * The IceTray I3Module wrapper around the central algorithm HiveSplitter and TriggerSplitter,
 * which in turn use a API trough SubEventStartStop
 */

#ifndef I3HIVECLUSTERING_H
#define I3HIVECLUSTERING_H

#include "icetray/I3ConditionalModule.h"

#include "IceHiveZ/algorithms/HiveSplitter.h"
#include "IceHiveZ/internals/Configurator.h"
#include "IceHiveZ/internals/Connector.h"

#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/I3MapOMKeyMask.h"

#include "ToolZ/I3RUsageTimer.h"

///The main module which drives the algorithms HiveSplitter for Clustering
class I3HiveClustering : public I3ConditionalModule {
  SET_LOGGER("I3HiveClustering");

protected://parameters
  //========================
  // Configurable Parameters
  //========================
  /// PARAM: which stream to execute on
  I3Frame::Stream stream_;
  /// PARAM: Name of the pulses to split in the frame
  std::string inputName_;
  /// PARAM: Name of the pulses to put in the split frames
  std::string outputName_;
  ///Params which are delivered to HiveSplitter
  hivesplitter::HiveSplitter_ParameterSet hs_param_set_;
  ///PARAM: A block with  HiveConfigurationServices; needs to be configured before passed on to HiveSplitter
  ConfiguratorBlockPtr configuratorBlock_;
  
  /// derived param: the connection services can be rebuild
  bool reconfigurable_;
                  
private: //bookkeeping  
  ///number of q-frames processed
  uint64_t n_frames_;
  ///number of hits processed
  uint64_t n_hits_in_;
  ///number of splits produced
  uint64_t n_splits_;
  ///number of hits produced
  uint64_t n_hits_out_;
  ///stopwatch
  I3RUsageTimer totRUsageDAQTimer_;
  ///stopwatch for splitter
  I3RUsageTimer totRUsageHiveSplitterTimer_;

private: //properties and methods related to configuration
  ///most private HiveSplitter instance
  HiveSplitter* hiveSplitter_;

  /// has any of the vital objects changed?
  bool configuration_changed_;
  /// store if we have seen all necessary frames GCD
  bool G_frame_seen_, C_frame_seen_, D_frame_seen_;
  
  /** @brief (Re)configure the splitters with new GCD objects
   * @param frame the frame that should have GCD objects present
   */
  void ConfigureSplitters(const I3FrameConstPtr frame);
  
public: //methods
  //================
  // Main Interface
  //================
  /// Constructor: configure Default values, register Parameters, register Outbox
  I3HiveClustering(const I3Context& context);
  
  /// Destructor
  virtual ~I3HiveClustering();

  /// Finish routine printing a little bit of information 
  void Finish();
  
  /// Configure Method to interact with icetray
  void Configure();

  /// DAQ call Method to interact with icetray
  void PerformClustering(I3FramePtr frame);

  /// Geometry call method to interact with Geometry-frame
  void Geometry(I3FramePtr frame);
  /// Calibration call method to interact with Calibration-frame
  void Calibration(I3FramePtr frame);
  /// DetectorStatus call method to interact with Status-frame
  void DetectorStatus(I3FramePtr frame);
};

/// Instantiate IceHive for the use with I3RecoPulses, aka working on I3RecoPulseSeriesMap(Masks)
I3_MODULE(I3HiveClustering);

#endif //I3HIVEClUSTER_H