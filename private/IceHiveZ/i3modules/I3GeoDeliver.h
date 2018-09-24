/**
 * \file I3GeoDeliver.h
 *
 * (c) 2012 the IceCube Collaboration
 *
 * $Id: I3IceHive.cxx 144968 2016-04-20 22:52:24Z mzoll $
 * \version $Revision: 144968 $
 * \date $Date: 2016-04-21 00:52:24 +0200 (tor, 21 apr 2016) $
 * \author Marcel Zoll <marcel.zoll@fysik.su.se>
 * 
 * A simple module that can deliver an idealised Geometry 
 * on the first call to Process()
 */

#ifndef I3GEODELIVER_H
#define I3GEODELIVER_H

#include "icetray/I3Module.h"
#include "dataclasses/geometry/I3Geometry.h"

//========= CLASS I3GeoDeliver ============

/// A Generator Module for to make a I3Geometry
class I3GeoDeliver: public I3Module{
  std::string geometry_name_;
public: //interaction with the tray
  I3GeoDeliver(const I3Context& context);
  void Configure();
  void Process();
};

I3_MODULE(I3GeoDeliver);

#endif // I3GEODELIVER_H
