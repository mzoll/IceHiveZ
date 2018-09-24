/**
 * \file IC86Topology.cxx
 *
 * (c) 2013 the IceCube Collaboration
 *
 * $Id: IceHiveHelpers.cxx 144357 2016-04-07 11:49:02Z mzoll $
 * \version $Revision: 144357 $
 * \date $Date: 2016-04-07 13:49:02 +0200 (tor, 07 apr 2016) $
 * \author mzoll <marcel.zoll@fysik.su.se>
 *
 * Pybindings to the IceHiveHelpers
 */

#include <boost/python.hpp>

#include "IceHiveZ/tools/IC86Topology.h"

#include <boost/make_shared.hpp>

namespace bp = boost::python;


namespace pyIC86Topology {
  I3GeometryConstPtr pyBuild_IC86_Geometry() {
    return boost::make_shared<const I3Geometry>(IC86Topology::Build_IC86_Geometry());
  };
};

void register_IC86Topology() {

  bp::def("is_icetop", &IC86Topology::IsIceTop
    , (bp::arg("omkey"))
    , "Is this in IceTop?"
  );
  
  bp::def("is_inice", &IC86Topology::IsInIce
    , (bp::arg("omkey"))
    , "Is this omkey in ice?"
  );

  bp::def("is_icecube", &IC86Topology::IsIceCube
    , (bp::arg("omkey"))
    , "Is this in regular IceCube?"
  );
  
  bp::def("is_deepcore", &IC86Topology::IsDeepCore
    , (bp::arg("omkey"))
    , "Is this in DeepCore?"
  );
      
  bp::def("is_deepcorecap", &IC86Topology::IsDeepCoreCap
    , (bp::arg("omkey"))
    , "Is this in the DeepCore Veto layer?"
  );
  
  bp::def("is_deepcorefidutial", &IC86Topology::IsDeepCoreFidutial
    , (bp::arg("omkey"))
    , "Is this in the denser populated DeepCore region?"
  );
    
  bp::def("is_deepcoredense", &IC86Topology::IsDeepCoreDense
    , (bp::arg("omkey"))
    , "Is this in the any dense populated region?"
  );
  
  bp::def("IC86_Geometry",
          &IC86Topology::Build_IC86_Geometry,
          "Build the I3Geometry of IC86 as an idealistic detector configuration ");

  bp::def("IC86_OMTopology",
          &IC86Topology::Build_IC86_OMKeyTopologyMap,
          "Build the OMTopologyMap for IC86 with the most common applications");

};

