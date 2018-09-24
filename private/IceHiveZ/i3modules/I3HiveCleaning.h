/**
 * \file I3HiveCleaning.h
 *
 * (c) 2012 the IceCube Collaboration
 *
 * $Id: I3HiveCleaning.h 99900 2013-02-26 10:10:43Z mzoll $
 * \version $Revision: 99900 $
 * \date $Date: 2013-02-26 11:10:43 +0100 (Tue, 26 Feb 2013) $
 * \author Marcel Zoll <marcel.zoll@fysik.su.se>
 *
 * The IceTray I3Module wrapper around the central algorithm HiveCleaning
 */

#ifndef I3HIVECLEANING_H
#define I3HIVECLEANING_H

#include "icetray/I3ConditionalModule.h"

#include "IceHiveZ/algorithms/HiveCleaning.h"
#include "IceHiveZ/internals/Configurator.h"
#include "IceHiveZ/internals/Connector.h"

#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/I3MapOMKeyMask.h"

#include "ToolZ/I3RUsageTimer.h"


///The main icetray module
class I3HiveCleaning : public I3ConditionalModule {
  SET_LOGGER("I3HiveCleaning");
  
protected://parameters
  //========================
  // Configurable Parameters
  //========================
  /// PARAM: which stream to execute on
  I3Frame::Stream stream_;
  /// PARAM: Name of the pulses to clean in the frame
  std::string inputName_;
  /// PARAM: Name of the cleaned pulses to put into the frame
  std::string outputName_;
  /// PARAMs which are delivered to HiveCleaning
  HiveCleaning_ParameterSet hc_param_set_;
  ///PARAM: A block with  HiveConfigurationServices; needs to be configured before passed on to HiveSplitter
  ConfiguratorBlockPtr configuratorBlock_;
  
  /// derived param: the connection services can be rebuild
  bool reconfigurable_;
  
private://properties
  ///most private HiveCleaning instance
  HiveCleaning* hiveCleaning_;
  
private: //properties and methods related to configuration
  /// has any of the vital objects changed?
  bool configuration_changed_;
  /// store if we have seen all necessary frames GCD
  bool G_frame_seen_, C_frame_seen_, D_frame_seen_;

  private: //bookkeeping  
  //number of p-frames passed by
  uint64_t n_frames_;
  //number of empty pulse-series produced
  uint64_t n_empty_;
  ///number of hits processed
  uint64_t n_hits_in_;
  ///number of splits produced
  uint64_t n_hits_out_;
  ///stopwatch for Configuration time
  I3RUsageTimer totRUsageConfigureTimer_;
  ///stopwatch for DAQ-call
  I3RUsageTimer totRUsageStreamTimer_;
  ///stopwatch for splitter
  I3RUsageTimer totRUsageHiveCleaningTimer_;
  
private://methods
  /** Reconfigure the subordinated modules
   * @param frame a frame containing the GCD objects, as forward propagated hand-downs
   */
  void Reconfigure(const I3FramePtr& frame);
  /// General call method to interact with icetray : Needs to be hooked up to an execution stream
  void PerformCleaning(I3FramePtr frame);

public://methods
  //================
  // Main Interface
  //================
  /// Constructor: configure Default values, register Parameters, register Outbox
  I3HiveCleaning(const I3Context& context);
  ///Destructor
  ~I3HiveCleaning();
  
  /// Configure method to interact with icetray
  void Configure();

  /// Geometry call method to interact with GCD-frames
  void Geometry(I3FramePtr frame);
  void Calibration(I3FramePtr frame);
  void DetectorStatus(I3FramePtr frame);
  
  ///report some numbers of interest
  void Finish();
};

I3_MODULE(I3HiveCleaning);

#endif //I3HIVECLEANING_H
