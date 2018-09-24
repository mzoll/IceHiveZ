/**
 * \file I3HiveSplitter.cxx
 *
 * (c) 2012 the IceCube Collaboration
 *
 * $Id: I3HiveSplitter.cxx 154506 2017-03-30 13:35:54Z mzoll $
 * \version $Revision: 154506 $
 * \date $Date: 2017-03-30 15:35:54 +0200 (Thu, 30 Mar 2017) $
 * \author Marcel Zoll <marcel.zoll@fysik.su.se>
 */

#include "IceHiveZ/i3modules/I3HiveSplitter.h"

#include "icetray/I3Units.h"
#include "icetray/I3Int.h"
#include "dataclasses/physics/I3EventHeader.h"
#include "ToolZ/HitFacility.h"
#include "ToolZ/ResponseMapHelpers.h"
#include "ToolZ/TriggerHierarchyHelpers.h"

//===============class I3IceHive=================================
I3HiveSplitter::I3HiveSplitter(const I3Context& context):
  I3ConditionalModule(context),
  I3Splitter(configuration_),
  inputName_(),
  outputName_(),
  trigHierName_("I3TriggerHierarchy"),
  updateTrigHierarchy_(false),
  //parameter sets for subordinated modules
  hs_param_set_(),
  //derived parameters
  readWindowMinus_(4000./I3Units::ns),
  readWindowPlus_(6000./I3Units::ns),
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
  
  AddParameter(
    "SubEventStreamName",
    "Name of the subEventStream the P-frames are written to",
    sub_event_stream_name_);
  AddParameter(
    "InputName",
    "Name of the input pulses in the Q-frame",
    inputName_);
  AddParameter(
    "OutputName",
    "OutputName of the processed pulses",
    outputName_);
  AddParameter(
    "TrigHierName",
    "Name to the TriggerHierarchy found in teh Q-frame",
    trigHierName_);
  AddParameter(
    "UpdateTriggerHierarchy",
    "Should the TriggerHierarchy by updated",
    updateTrigHierarchy_);
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
    "ReadoutWindowMinus",
    "Length of readout window to pad before event/trigger in [ns]. This time is used in the TriggerSplitter pulse extraction and is added to the '_Noised' pulse output for every subevent.",
    readWindowMinus_);
  AddParameter(
    "ReadoutWindowPlus",
    "Length of readout window to pad after end of event/trigger in [ns]. This time is used in the TriggerSplitter pulse extraction and is added to the '_Noised' pulse output for every subevent.",
    readWindowPlus_);
  AddParameter(
    "ConfiguratorBlock",
    "A block of Configurators to build a ConnectorBlock from",
    configuratorBlock_);
  AddParameter(
    "ConnectorBlock",
    "A block of preconfigured Connectors",
    hs_param_set_.connectorBlock);
  
  AddOutBox("OutBox");

  log_info("This is I3HiveSplitter!");
  log_debug("Leaving Init()");
}


I3HiveSplitter::~I3HiveSplitter() {
  if (hiveSplitter_!=NULL)
    delete hiveSplitter_;
}


void I3HiveSplitter::Configure() {
  log_debug("Entering Configure()");
  GetParameter("SubEventStreamName", sub_event_stream_name_);
  GetParameter("InputName", inputName_);
  GetParameter("OutputName", outputName_);
  GetParameter("UpdateTriggerHierarchy", updateTrigHierarchy_);
  GetParameter("Multiplicity", hs_param_set_.multiplicity);
  GetParameter("MultiplicityTimeWindow", hs_param_set_.multiplicityTimeWindow);
  GetParameter("AcceptTimeWindow", hs_param_set_.acceptTimeWindow);
  GetParameter("RejectTimeWindow", hs_param_set_.rejectTimeWindow);
  GetParameter("MergeOverlap", hs_param_set_.mergeOverlap);
  GetParameter("TrigHierName", trigHierName_);
  GetParameter("ReadoutWindowMinus", readWindowMinus_);
  GetParameter("ReadoutWindowPlus", readWindowPlus_);
  
  GetParameter("ConfiguratorBlock", configuratorBlock_);
  GetParameter("ConnectorBlock", hs_param_set_.connectorBlock);
  
  if (configuratorBlock_ && hs_param_set_.connectorBlock)
    log_fatal("Can work only either 'ConfiguratorBlock' or 'ConnectorBlock'");
  
  if (configuratorBlock_)
    reconfigurable_= true;

  if (hs_param_set_.connectorBlock) {
    reconfigurable_= false;
    hiveSplitter_ = new HiveSplitter( hs_param_set_ );
  }
  
  if (sub_event_stream_name_.empty())
    log_fatal("Configure the Name of the SubEventStreamName");
  
  if (inputName_.empty())
    log_fatal("Configure the Name of the Input");
  if (outputName_.empty())
    log_fatal("Configure the Name of the Output");
  if (trigHierName_.empty())
    log_fatal("Configure the Name of the TriggerHierarchy");

  if (!updateTrigHierarchy_)
    log_warn("The TriggerHierarchy will not be updated");
  
  log_debug("Leaving Configure()");
}


void I3HiveSplitter::Finish(){
  log_debug("Entering Finish()");
  log_notice("%s: Processed %lu(/%lu) frames(/hits) producing %lu(/%lu) splits(/hits)",
    GetName().c_str(),
    n_frames_,
    n_hits_in_,
    n_splits_,
    n_hits_out_);
  
  I3RUsagePtr totalRUsage = totRUsageDAQTimer_.GetTotalRUsage();
  log_notice(
      "%s: %lu calls to DAQ: %s, %.2fms(/%.1fus) per frame(/hit) processed, %.2fms(/%.1fus) per subevent(/hit) produced",
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


void I3HiveSplitter::Geometry(I3FramePtr frame){
  log_debug("Entering Geometry()");
  G_frame_seen_ = true;
  configuration_changed_ = true;
  PushFrame(frame);
}


void I3HiveSplitter::Calibration(I3FramePtr frame){
  log_debug("Entering Calibration()");
  C_frame_seen_ = true;
  configuration_changed_ = true;
  PushFrame(frame);
}


void I3HiveSplitter::DetectorStatus(I3FramePtr frame){
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


void I3HiveSplitter::ConfigureSplitters(const I3FrameConstPtr frame) {
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
  
  const GCDinfo gcdinfo(geometry, calibration, status);
  
  if (reconfigurable_) {
    //reset everything and compute from scratch
    totRUsageConfigureTimer_.Start();
    
    hs_param_set_.connectorBlock 
      = boost::make_shared<ConnectorBlock>(configuratorBlock_->BuildConnectorBlock(geometry->omgeo));
      
    totRUsageConfigureTimer_.Stop();
    I3RUsagePtr totalRUsage = totRUsageConfigureTimer_.GetTotalRUsage();
    log_notice("%s: call to Reconfigure: %s",
      GetName().c_str(),
      convertI3RUsageToString(*totalRUsage).c_str());
    totRUsageConfigureTimer_.ResetTotalTimes();
    //destroy the old splitter, instaniate a new one
    delete hiveSplitter_;
    hiveSplitter_ = new HiveSplitter( hs_param_set_ );
  }
  else {
    log_info("New Geometry has passed but will be ignored because of preconfigured ConnetorBlock");
  }
};


void
I3HiveSplitter::AugmentTriggerHierarchy (I3TriggerHierarchy& trigHier,
                                        const int start_time,
                                        const int end_time,
                                        const int configID) const 
{
  I3Trigger hsTrigger; //create the HiveSplitter-trigger
  hsTrigger.GetTriggerKey() = TriggerKey(TriggerKey::IN_ICE, TriggerKey::FRAGMENT_MULTIPLICITY, configID);
  hsTrigger.SetTriggerFired(true);
  hsTrigger.SetTriggerTime(start_time);
  hsTrigger.SetTriggerLength(end_time - start_time);

  I3Trigger tpTrigger; //create the individual throughput trigger associated to the HiveSplitter-trigger
  tpTrigger.GetTriggerKey() = TriggerKey(TriggerKey::GLOBAL, TriggerKey::THROUGHPUT);
  tpTrigger.SetTriggerFired(true);
  tpTrigger.SetTriggerTime(start_time-readWindowMinus_);
  tpTrigger.SetTriggerLength((end_time+readWindowPlus_) - (start_time-readWindowMinus_));
  //append the throughput trigger to the global trigger
  I3TriggerHierarchy::iterator eachTrig = trigHier.append_child(trigHier.end(), tpTrigger);
  //insert actual trigger as child of the individual throughput
  trigHier.append_child(eachTrig, hsTrigger);
};


void I3HiveSplitter::DAQ (I3FramePtr frame) {
  log_debug("Entering DAQ()");
  using namespace HitSorting;
  
  if (configuration_changed_) { //this is a precaution if a late reconfiguring is needed, when only single G,C,D frame was passed
    log_info("Reconfiguring the Connectors after GCD has changed");
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
  const AbsHitSet hits(hitFacility.GetAbsHits<AbsHitSet>());
  n_hits_in_+=hits.size();
  
  log_debug("perform the splitting");
  totRUsageHiveSplitterTimer_.Start(); //start the watch
  AbsHitSetSequence subEvents = hiveSplitter_->Split(hits);
  totRUsageHiveSplitterTimer_.Stop(); //start the watch

  log_debug("interpret the subeventclusters");
  //Save the SplitCount if so requested
  
  //Some users are concerned with how many subevents we created, so let's save them some trouble
  const unsigned int nSubEvents=subEvents.size();
  frame->Put(sub_event_stream_name_+"SplitCount", boost::make_shared<I3Int>(nSubEvents));

  // create a mask holding all hits from all the subevents at once, and the inversion of it
  AbsHitSet unitedHits;
  BOOST_FOREACH(const AbsHitSet &hits, subEvents) {
    unitedHits.insert(hits.begin(), hits.end());
  }
  
  //put all clusters into the frame
  int cluster_count = 0;
  BOOST_FOREACH(const AbsHitSet &hits, subEvents) {
    std::stringstream ss;
    ss << outputName_ << "_Cluster" << std::setw(2) << std::setfill('0') << cluster_count;
    
    frame->Put(ss.str(), boost::make_shared<I3RecoPulseSeriesMapMask>(hitFacility.MaskFromAbsHits(hits)));
    cluster_count++;
  }
  
  const I3RecoPulseSeriesMapMask unitedMask = hitFacility.MaskFromAbsHits(unitedHits);
 
  I3RecoPulseSeriesMapMask noiseMask(*frame, inputName_, *(frame->Get<I3RecoPulseSeriesMapConstPtr>(inputName_)));
  noiseMask = noiseMask ^ unitedMask; //invert the mask

  frame->Put(outputName_+"_Physics", boost::make_shared<I3RecoPulseSeriesMapMask>(unitedMask));
  frame->Put(outputName_+"_Noise", boost::make_shared<I3RecoPulseSeriesMapMask>(noiseMask));

  //need to delay until here
  PushFrame(frame); // push the Q-frame
  
  //process the subevents, put them each into a new subFrame and augment with further objects
  BOOST_FOREACH(const AbsHitSet &subEvent, subEvents) {
    n_hits_out_+=subEvent.size();
    I3FramePtr subframe = GetNextSubEvent(frame);
    //modify the I3EventHeader of the newly created subframe
    I3EventHeader eh = *subframe->Get<I3EventHeaderConstPtr>("I3EventHeader");
    const double &rel_start_time = subEvent.begin()->GetTime();
    const double &rel_end_time = subEvent.rbegin()->GetTime();
    const I3Time abs_start_time = eh.GetStartTime();
    eh.SetStartTime(abs_start_time+rel_start_time);
    eh.SetEndTime(abs_start_time+rel_end_time);
    subframe->Delete("I3EventHeader");
    subframe->Put("I3EventHeader", boost::make_shared<I3EventHeader>(eh));
    
    //convert the subevent Hit set into a proper mask on the pulses series
    const I3RecoPulseSeriesMapMask subevent_mask = hitFacility.MaskFromAbsHits(subEvent);
    subframe->Put(outputName_, boost::make_shared<I3RecoPulseSeriesMapMask>(subevent_mask));
    //also create a separate object holding start and end times of the event
    const I3TimeWindow subevent_time_window(subEvent.begin()->GetTime(), subEvent.rbegin()->GetTime());
    subframe->Put(outputName_ + "_TimeRange", boost::make_shared<I3TimeWindow>(subevent_time_window));

    //Get me the noise within the time window of that subevent; this is an overlay
    I3RecoPulseSeriesMapConstPtr noisePulses = noiseMask.Apply(*frame);
    
    const I3TimeWindow noised_time_window(subEvent.begin()->GetTime()-readWindowMinus_,
                                          subEvent.rbegin()->GetTime()+readWindowPlus_);
    
    const I3RecoPulseSeriesMap noiseInRange = GetResponsesInTimeRange (*noisePulses, noised_time_window);
    const I3RecoPulseSeriesMapMask mask_noiseInRange(*frame, noiseMask.GetSource(), noiseInRange);

    const I3RecoPulseSeriesMapMask noised_mask = subevent_mask | mask_noiseInRange;
    subframe->Put(outputName_+"_Noised", boost::make_shared<I3RecoPulseSeriesMapMask>(noised_mask));

    if (updateTrigHierarchy_) {
      // Get the trigger times and lengths
      I3TriggerHierarchyConstPtr trigHier = frame->Get<I3TriggerHierarchyConstPtr>(trigHierName_);
      if(!trigHier) {
        log_error_stream("Could not locate the <I3TriggerHierarchy>'"<<trigHierName_ <<"' in the frame; TriggerHierarchy wont be updated");
      }
      
      if (trigHier) {
        I3TriggerHierarchy trigHier_clipped = 
          ClipTriggerHierarchy(*trigHier, noised_time_window);
        //when this option is selected clip the Trigger Hierarchy to the according time-range and insert a Trigger for HiveSplitter
//         AugmentTriggerHierarchy(trigHier_clipped,
//           subEvent.begin()->GetTime(),
//           subEvent.rbegin()->GetTime());
        subframe->Put(trigHierName_+"_clipped", boost::make_shared<I3TriggerHierarchy>(trigHier_clipped));
      }
    }
    n_splits_++;
    
    PushFrame(subframe); //push the subevent P-frame 
  }
  n_frames_++;
  
  totRUsageDAQTimer_.Stop();
  log_debug("Leaving DAQ()");
};


