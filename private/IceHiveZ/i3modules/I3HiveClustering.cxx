/**
 * \file I3HiveClustering.cxx
 *
 * (c) 2012 the IceCube Collaboration
 *
 * $Id: I3HiveClustering.cxx 153959 2017-03-17 13:43:24Z mzoll $
 * \version $Revision: 153959 $
 * \date $Date: 2017-03-17 14:43:24 +0100 (Fri, 17 Mar 2017) $
 * \author Marcel Zoll <marcel.zoll@fysik.su.se>
 */

#include "IceHiveZ/i3modules/I3HiveClustering.h"

#include "icetray/I3Units.h"
#include "icetray/I3Int.h"
#include "ToolZ/HitFacility.h"

//===============class I3IceHive=================================
I3HiveClustering::I3HiveClustering(const I3Context& context):
  I3ConditionalModule(context),
  stream_(I3Frame::Physics),
  inputName_(),
  outputName_(),
  //parameter sets for subordinated modules
  hs_param_set_(),
  //for configuration
  configuratorBlock_(),
  reconfigurable_(false),
  //bookkeeping
  n_frames_(0),
  n_hits_in_(0),
  n_splits_(0),
  n_hits_out_(0),
  //initialize the splitter algorithms
  hiveSplitter_(NULL),
  //do we need to reconfigure?
  configuration_changed_(true),
  G_frame_seen_(false),
  C_frame_seen_(false),
  D_frame_seen_(false)
{
  log_debug("Creating I3IceHive instance");
  
  AddParameter("Stream",
               "The Stream to execute on",
               stream_);
  AddParameter(
    "InputName",
    "Name of the input pulses in the Q-frame",
    inputName_);
  AddParameter(
    "OutputName",
    "OutputName of the processed pulses",
    outputName_);
  AddParameter(
    "Multiplicity",
    "Required multiplicity in causal connected hits to form a subevent",
    hs_param_set_.multiplicity);
  AddParameter(
    "MultiplicityTimeWindow",
    "Time span within which the multiplicity requirement must be met in [ns]",
    hs_param_set_.multiplicityTimeWindow);
  AddParameter(
    "AcceptTimeWindow",
    "Connect all hits on same DOM up to this time limit after the initial hit regardlessly [ns]; deactivate by NAN",
    hs_param_set_.acceptTimeWindow);
  AddParameter(
    "RejectTimeWindow",
    "Reject all hits on same DOM from to this time limit after the initial hit regardlessly [ns]; deactivate by INF ",
    hs_param_set_.rejectTimeWindow);
  AddParameter(
    "MergeOverlap",
    "Number of overlapping DOMs required for (partial) subevents to be merged",
    hs_param_set_.mergeOverlap);
  AddParameter(
    "ConfiguratorBlock",
    "A block of Configurators to build a ConnectorBlock from",
    configuratorBlock_);
  AddParameter(
    "ConnectorBlock",
    "A block of preconfigured Connectors",
    hs_param_set_.connectorBlock);
  AddOutBox("OutBox");

  log_info("This is I3HiveClustering!");
  log_debug("Leaving Init()");
}


I3HiveClustering::~I3HiveClustering() {
  if (hiveSplitter_!=NULL)
    delete hiveSplitter_;
}


void I3HiveClustering::Configure() {
  log_debug("Entering Configure()");
  GetParameter("Stream", stream_);
  GetParameter("InputName", inputName_);
  GetParameter("OutputName", outputName_);
  GetParameter("Multiplicity", hs_param_set_.multiplicity);
  GetParameter("MultiplicityTimeWindow", hs_param_set_.multiplicityTimeWindow);
  GetParameter("AcceptTimeWindow", hs_param_set_.acceptTimeWindow);
  GetParameter("RejectTimeWindow", hs_param_set_.rejectTimeWindow);
  GetParameter("MergeOverlap", hs_param_set_.mergeOverlap);
  
  GetParameter("ConfiguratorBlock", configuratorBlock_);
  GetParameter("ConnectorBlock", hs_param_set_.connectorBlock);
  
  if (configuratorBlock_ && hs_param_set_.connectorBlock)
    log_fatal("Can do only either 'ConfiguratorBlock' or 'ConnectorBlock'");
  
  if (configuratorBlock_)
    reconfigurable_= true;

  if (hs_param_set_.connectorBlock) {
    reconfigurable_= false;
    hiveSplitter_ = new HiveSplitter( hs_param_set_ );
  }
  
  if (inputName_.empty())
    log_fatal("Configure the Name of the Input");
  if (outputName_.empty())
    log_fatal("Configure the Name of the Output");
  
  Register(stream_, &I3HiveClustering::PerformClustering);

  log_debug("Leaving Configure()");
}


void I3HiveClustering::Finish(){
  log_debug("Entering Finish()");
  log_notice("%s: Processed %lu(/%lu) frames(/hits) producing %lu(/%lu) splits(/hits)",
    GetName().c_str(),
    n_frames_,
    n_hits_in_,
    n_splits_,
    n_hits_out_);
  
  I3RUsagePtr totalRUsage = totRUsageDAQTimer_.GetTotalRUsage();
  log_notice(
      "%s: %lu calls to Process: %s, %.2fms(/%.1fus) per frame(/hit) processed, %.2fms(/%.1fus) per subevent(/hit) produced",
      GetName().c_str(),
      n_frames_,
      convertI3RUsageToString(*totalRUsage).c_str(),
      (n_frames_ ? totalRUsage->wallclocktime/I3Units::millisecond/n_frames_ : 0),
      (n_hits_in_ ? totalRUsage->wallclocktime/I3Units::microsecond/n_hits_in_ : 0),
      (n_splits_ ? totalRUsage->wallclocktime/I3Units::millisecond/n_splits_ : 0),
      (n_hits_out_ ? totalRUsage->wallclocktime/I3Units::microsecond/n_hits_out_ : 0));
  
  totalRUsage = totRUsageHiveSplitterTimer_.GetTotalRUsage();
  log_notice(
      "%s: %lu calls to HiveSplitter: %s, %.2fms(/%.1fus) per frame(/hit) processed, %.2fms(/%.1fus) per subevent(/hit) produced",
      GetName().c_str(),
      n_frames_,
      convertI3RUsageToString(*totalRUsage).c_str(),
      (n_frames_ ? totalRUsage->wallclocktime/I3Units::millisecond/n_frames_ : 0),
      (n_hits_in_ ? totalRUsage->wallclocktime/I3Units::microsecond/n_hits_in_ : 0),
      (n_splits_ ? totalRUsage->wallclocktime/I3Units::millisecond/n_splits_ : 0),
      (n_hits_out_ ? totalRUsage->wallclocktime/I3Units::microsecond/n_hits_out_ : 0));
  
  I3Module::Finish();
};


void I3HiveClustering::Geometry(I3FramePtr frame){
  log_debug("Entering Geometry()");
  G_frame_seen_ = true;
  configuration_changed_ = true;
  PushFrame(frame);
}


void I3HiveClustering::Calibration(I3FramePtr frame){
  log_debug("Entering Calibration()");
  C_frame_seen_ = true;
  configuration_changed_ = true;
  PushFrame(frame);
}


void I3HiveClustering::DetectorStatus(I3FramePtr frame){
  log_debug("Entering DetectorStatus()");
  D_frame_seen_ = true;
  configuration_changed_ = true;
  // NOTE reconfigure the module when a complete GCD has passed:
  //normally reconfiguration should happen when the configuration has changed,
  //but the D frame is frequently the last frame of a GCD file and all inforamtion
  //neccessary for configuration is therefore provided
  if (G_frame_seen_ && C_frame_seen_ && D_frame_seen_) {
    ConfigureSplitters(frame);
    configuration_changed_ = false;
  }
  PushFrame(frame);
}


void I3HiveClustering::ConfigureSplitters(const I3FrameConstPtr frame) {
  log_debug("Entering ConfigureSplitters()");

  I3GeometryConstPtr geometry;
  I3CalibrationConstPtr calibration;
  I3DetectorStatusConstPtr status;
  
  geometry= frame->Get<I3GeometryConstPtr>();
  if (!geometry)
    log_fatal("Unable to find Geometry data!");
  //the following are in principle not needed for the functionallity of the splitter
  calibration= frame->Get<I3CalibrationConstPtr>();
  if (! calibration)
    log_warn("Unable to find Calibration data!");
  status = frame->Get<I3DetectorStatusConstPtr>();
  if (! status)
    log_warn("Unable to find DetectorStatus data!");
  
  if (reconfigurable_) {
    //reset everything and compute from scratch    hs_param_set_.connectorBlock 
    hs_param_set_.connectorBlock 
      = boost::make_shared<ConnectorBlock>(configuratorBlock_->BuildConnectorBlock(geometry->omgeo));
    //destroy the old splitter, instaniate a new one
    delete hiveSplitter_;
    hiveSplitter_ = new HiveSplitter( hs_param_set_ );
  }
  else {
    log_info("New Geometry has passed but will be ignored because of preconfigured ConnetorBlock");
  }
};


void I3HiveClustering::PerformClustering (I3FramePtr frame) {
  log_debug("Entering DAQ()");
  using namespace HitSorting;
  
  if (configuration_changed_) { //this is a precaution if a late reconfiguring is needed, when only single G,C,D frame was passed
    log_error("Reconfiguring the Connectors after GCD has changed");
    ConfigureSplitters(frame);
    configuration_changed_= false;
  }
  
  //fetch inputs
  if (!frame->Has(inputName_)) {
    log_error_stream("Could not locate the key '"<<inputName_ <<"' in the frame; nothing to be done");
    PushFrame(frame);
    return;
  }

  totRUsageDAQTimer_.Start(); //start the watch
  
  //turn the crank:
  log_debug("create hitFacility");
  I3RecoPulseSeriesMap_HitFacility hitFacility(frame, inputName_, hs_param_set_.connectorBlock->GetHashService());

  log_debug("extract pulses");
  const AbsHitSet hits = hitFacility.GetAbsHits<AbsHitSet>();
  n_hits_in_+=hits.size();
  
  log_debug("perform the splitting");
  totRUsageHiveSplitterTimer_.Start(); //start the watch
  AbsHitSetSequence subEvents = hiveSplitter_->Split(hits);
  totRUsageHiveSplitterTimer_.Stop(); //start the watch

  log_debug("Interpret the subeventclusters");

  // create a mask holding all hits from all the subevents at once, and the inversion of it
  AbsHitSet unitedHits;
  BOOST_FOREACH(const AbsHitSet &hits, subEvents) {
    unitedHits.insert(hits.begin(), hits.end());
    n_hits_out_+=hits.size();
    n_splits_++;
  }

  const I3RecoPulseSeriesMapMask unitedMask = hitFacility.MaskFromAbsHits(unitedHits);
 
  I3RecoPulseSeriesMapMask noiseMask(*frame, inputName_, *(frame->Get<I3RecoPulseSeriesMapConstPtr>(inputName_)));
  noiseMask = noiseMask ^ unitedMask; //invert the mask

  frame->Put(outputName_+"_Physics", boost::make_shared<I3RecoPulseSeriesMapMask>(unitedMask));
  frame->Put(outputName_+"_Noise", boost::make_shared<I3RecoPulseSeriesMapMask>(noiseMask));
  frame->Put(outputName_+"_NClusters", boost::make_shared<I3Int>(subEvents.size()));
  
  int cluster_count = 0;
  BOOST_FOREACH(const AbsHitSet &hits, subEvents) {
    std::stringstream ss;
    ss << outputName_ << "_Cluster" << std::setw(2) << std::setfill('0') << cluster_count;
    
    frame->Put(ss.str(), boost::make_shared<I3RecoPulseSeriesMapMask>(hitFacility.MaskFromAbsHits(hits)));
    cluster_count++;
  }
    
  //need to delay until here
  PushFrame(frame); // push the Q-frame
  n_frames_++;
  
  totRUsageDAQTimer_.Stop();
  log_debug("Leaving PerformCLustering()");
};


