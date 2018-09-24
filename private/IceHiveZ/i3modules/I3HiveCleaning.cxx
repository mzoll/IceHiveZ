/**
 * \file I3HiveCleaning.cxx
 *
 * (c) 2012 the IceCube Collaboration
 *
 * $Id: I3HiveCleaning.cxx 153959 2017-03-17 13:43:24Z mzoll $
 * \version $Revision: 153959 $
 * \date $Date: 2017-03-17 14:43:24 +0100 (Fri, 17 Mar 2017) $
 * \author Marcel Zoll <marcel.zoll@fysik.su.se>
 */

#include "IceHiveZ/i3modules/I3HiveCleaning.h"

#include "icetray/I3Units.h"
#include "icetray/I3Int.h"
#include "dataclasses/physics/I3EventHeader.h"
#include "ToolZ/HitFacility.h"


//===============class I3HiveCleaning=================================

I3HiveCleaning::I3HiveCleaning(const I3Context& context):
  I3ConditionalModule(context),
  stream_(I3Frame::Physics),
  inputName_(""),
  outputName_(""),
  hc_param_set_(),
  //for configuration
  configuratorBlock_(),
  //initialize the splitter algorithms
  hiveCleaning_(NULL),
  //bookkeeping
  n_frames_(0),
  n_empty_(0),
  n_hits_in_(0),
  n_hits_out_(0)
{
  log_debug("Creating I3HiveCleaning instance");
  AddParameter("InputName",
               "Name of the input pulses in the Q-frame",
               inputName_);
  AddParameter("OutputName",
               "Name of the processed pulses will be written to",
               outputName_);
  AddParameter("Multiplicity",
               "Required multiplicity of connected neighbours",
               hc_param_set_.multiplicity);
  AddParameter("MaxTimeResidualEarly",
               "Maximum positive time which will allow a close-by pair of hits to be considered connected in [ns]",
               hc_param_set_.max_tresidual_early);
  AddParameter("MaxTimeResidualLate",
               "Maximum positive time which will allow a close-by pair of hits to be considered connected in [ns]",
               hc_param_set_.max_tresidual_early);
  AddParameter("ConfiguratorBlock",
               "A block of Configurators for to build a ConnectorBlock from",
               configuratorBlock_);
  AddParameter("ConnectorBlock",
               "A block of preconfigured Connectors",
               hc_param_set_.connectorBlock);
  AddParameter("Stream", "The Stream to execute on", stream_);
  AddOutBox("OutBox");

  log_info("This is I3HiveCleaning!");
  log_debug("Leaving Init()");
}


I3HiveCleaning::~I3HiveCleaning() {
  if (hiveCleaning_!=NULL)
    delete hiveCleaning_;
}


void I3HiveCleaning::Configure() {
  log_debug("Entering Configure()");
  GetParameter("InputName", inputName_);
  GetParameter("OutputName", outputName_);
  GetParameter("Multiplicity", hc_param_set_.multiplicity);
  GetParameter("MaxTimeResidualEarly", hc_param_set_.max_tresidual_early);
  GetParameter("MaxTimeResidualLate", hc_param_set_.max_tresidual_late);
  GetParameter("ConfiguratorBlock", configuratorBlock_);
  GetParameter("ConnectorBlock", hc_param_set_.connectorBlock);
  GetParameter("Stream", stream_);
  
  if (configuratorBlock_ && hc_param_set_.connectorBlock)
    log_fatal("can currently only do either 'IceHiveConfigBlock' or 'ConnectorBlock'");
  
  if (configuratorBlock_)
    reconfigurable_= true;

  if (hc_param_set_.connectorBlock) {
    reconfigurable_= false;
    hiveCleaning_ = new HiveCleaning( hc_param_set_ );
  }
  
  if (inputName_=="")
    log_fatal("Configure the Name of the Input!");
  if (outputName_=="")
    log_fatal("Configure the Name of the Output!");

  Register(stream_, &I3HiveCleaning::PerformCleaning);

  log_debug("Leaving Configure()");
}


void I3HiveCleaning::Geometry(I3FramePtr frame){
  log_debug("Entering Geometry()");
  G_frame_seen_ = true;
  configuration_changed_ = true;
  PushFrame(frame);
}


void I3HiveCleaning::Calibration(I3FramePtr frame){
  log_debug("Entering Calibration()");
  C_frame_seen_ = true;
  configuration_changed_ = true;
  PushFrame(frame);
}


void I3HiveCleaning::DetectorStatus(I3FramePtr frame){
  log_debug("Entering DetectorStatus()");
  D_frame_seen_ = true;
  configuration_changed_ = true;
  // NOTE reconfigure the module when a complete GCD has passed:
  //normally reconfiguration should happen when the configuration has changed,
  //but the D frame is frequently the last frame of a GCD file and all inforamtion
  //neccessary for configuration is therefore provided
  if (G_frame_seen_ && C_frame_seen_ && D_frame_seen_) {
    Reconfigure(frame);
    configuration_changed_= false;
  }
  PushFrame(frame);
}

void I3HiveCleaning::Finish() {
  log_debug("Entering Finish()");
  
  log_notice_stream("In "<<n_empty_ <<" of "<<n_frames_ <<" cases everything has been cleaned away.");
  log_notice("%s: Processed %lu(/%lu) frames(/hits) producing %lu(/%lu) events(/hits)",
    GetName().c_str(),
    n_frames_,
    n_hits_in_,
    n_frames_-n_empty_,
    n_hits_out_);
  
  I3RUsagePtr totalRUsage = totRUsageStreamTimer_.GetTotalRUsage();
  log_notice(
      "%s: %lu calls to Stream: %s, %.2fms(/%.1fus) per frame(/hit) processed, %.1fus per hit retained",
      GetName().c_str(),
      n_frames_,
      convertI3RUsageToString(*totalRUsage).c_str(),
      (n_frames_ ? totalRUsage->wallclocktime/I3Units::millisecond/n_frames_ : 0),
      (n_hits_in_ ? totalRUsage->wallclocktime/I3Units::microsecond/n_hits_in_ : 0),
      (n_hits_out_ ? totalRUsage->wallclocktime/I3Units::microsecond/n_hits_out_ : 0));
  
  totalRUsage = totRUsageHiveCleaningTimer_.GetTotalRUsage();
  log_notice(
      "%s: %lu calls to HiveCleaning: %s, %.2fms(/%.1fus) per frame(/hit) processed, %.1f per hit retained",
      GetName().c_str(),
      n_frames_,
      convertI3RUsageToString(*totalRUsage).c_str(),
      (n_frames_ ? totalRUsage->wallclocktime/I3Units::millisecond/n_frames_ : 0),
      (n_hits_in_ ? totalRUsage->wallclocktime/I3Units::microsecond/n_hits_in_ : 0),
      (n_hits_out_ ? totalRUsage->wallclocktime/I3Units::microsecond/n_hits_out_ : 0));
  
  I3Module::Finish();
};

void I3HiveCleaning::Reconfigure(const I3FramePtr& frame) {
  I3GeometryConstPtr geometry;
  I3CalibrationConstPtr calibration;
  I3DetectorStatusConstPtr status;
  
  geometry= frame->Get<I3GeometryConstPtr>();
  if (!geometry)
    log_fatal("Unable to find Geometry data!");
  calibration= frame->Get<I3CalibrationConstPtr>();
  if (! calibration)
    log_warn("Unable to find Calibration data!");
  status = frame->Get<I3DetectorStatusConstPtr>();
  if (! status)
    log_warn("Unable to find DetectorStatus data!");
  
  if (reconfigurable_) {
     //reset everything and compute from scratch

    hc_param_set_.connectorBlock 
      = boost::make_shared<ConnectorBlock>(configuratorBlock_->BuildConnectorBlock(geometry->omgeo));
    
    //destroy the old splitter, instaniate a new one
    delete hiveCleaning_;
    hiveCleaning_ = new HiveCleaning( hc_param_set_ );
  }
    else {
    log_info("New Geometry has passed but will be ignored because of preconfigured ConnetorBlock");
  }
}


void I3HiveCleaning::PerformCleaning (I3FramePtr frame) {
  log_debug("Entering PerformCleaning()");

  using namespace HitSorting;
  
  //fetch inputs
  if (!frame->Has(inputName_)) {
    log_error_stream("Could not locate the <RecoPulsesSeriesMap>("<<inputName_ <<") in the frame; nothing to be done");
    PushFrame(frame);
    return;
  }
  
  totRUsageStreamTimer_.Start(); //start the watch
  
  n_frames_++; //bookkeeping
  
  I3RecoPulseSeriesMap_HitFacility hitFacility(frame, inputName_, hc_param_set_.connectorBlock->GetHashService());
  
  const AbsHitSet hits = hitFacility.GetAbsHits<AbsHitSet>();
  n_hits_in_+= hits.size();
  
  //turn the crank
  log_debug("Performing Cleaning");
  totRUsageHiveCleaningTimer_.Start(); //start the watc  
  const AbsHitSet cleanedHits = hiveCleaning_->Clean(hits);
  totRUsageHiveCleaningTimer_.Stop(); //start the watc
  
  if (! cleanedHits.size()){
      log_debug("everything was cleaned away");
      n_empty_++;
  }
  n_hits_out_+= cleanedHits.size();
  
  frame->Put(outputName_, boost::make_shared<I3RecoPulseSeriesMapMask>(hitFacility.MaskFromAbsHits(cleanedHits)));
  
  PushFrame(frame);
  totRUsageStreamTimer_.Stop(); //stop the watch
};


