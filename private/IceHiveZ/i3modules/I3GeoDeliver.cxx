/**
 * \file I3GeoDeliver.cxx
 *
 * (c) 2012 the IceCube Collaboration
 *
 * $Id: I3IceHive.cxx 144968 2016-04-20 22:52:24Z mzoll $
 * \version $Revision: 144968 $
 * \date $Date: 2016-04-21 00:52:24 +0200 (tor, 21 apr 2016) $
 * \author Marcel Zoll <marcel.zoll@fysik.su.se>
 */

#include "IceHiveZ/i3modules/I3GeoDeliver.h"

#include "ToolZ/IC86Topology.h"

#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>


//========= CLASS I3GeoDeliver ============

I3GeoDeliver::I3GeoDeliver(const I3Context& context):
  I3Module(context),
  geometry_name_("IC86")
{
  AddParameter("GeometryName", "which Geometry? choose from: IC86", geometry_name_);
  AddOutBox("OutBox");
};

void I3GeoDeliver::Configure() {
  GetParameter("GeometryName", geometry_name_);
  if (geometry_name_!="IC86" && geometry_name_!="IC79" && geometry_name_!="IC59" && geometry_name_!="IC40")
    log_fatal("unknown geometry; choose from: IC86, IC79, IC59, IC40");
};

void I3GeoDeliver::Process() {
  I3FramePtr gframe= boost::make_shared<I3Frame>(I3Frame::Geometry);
  I3GeometryPtr geo;
  geo = boost::make_shared<I3Geometry>(IC86Topology::Build_IC86_Geometry());
  
  //remove strings from the geometry for older geometry settings
  if (geometry_name_=="IC86") 
    {}
  else if (geometry_name_=="IC79") {
    I3OMGeoMap::iterator iter=geo->omgeo.begin();
    while (iter!= geo->omgeo.end()) {
      const unsigned str = iter->first.GetString();
      if (str==1 || str==7 || str==14 || str==22 || str==31 || str==79 || str==80) {
        I3OMGeoMap::iterator iter_next= iter;
        iter_next++;
        geo->omgeo.erase(iter);
        iter = iter_next;
      }
      else
        ++iter;
    }
  }
  else if (geometry_name_=="IC59") {
    log_fatal("implement me");
    I3OMGeoMap::iterator iter=geo->omgeo.begin();
    while (iter!= geo->omgeo.end()) {
      const unsigned str = iter->first.GetString();
      if (str==1 || str==7 || str==14 || str==22 || str==31 || str==79 || str==80) {
        I3OMGeoMap::iterator iter_next= iter;
        iter_next++;
        geo->omgeo.erase(iter);
        iter = iter_next;
      }
      else
        ++iter;
    }
  }
  else
    log_fatal("unknown geometry");
  
  gframe->Put("I3Geometry", geo);
  PushFrame(gframe);
  RequestSuspension();
};
