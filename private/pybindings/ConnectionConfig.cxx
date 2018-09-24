/**
 * copyright  (C) 2012
 * the IceCube Collaboration
 * $Id: $
 *
 * @file HiveConfig.cxx
 * @date $Date: 2012-12-20$
 * @author mzoll <marcel.zoll@fysik.su.se>
 */

#include <string>

#include "icetray/python/stream_to_string.hpp"
#include "dataclasses/ostream_overloads.hpp"
#include "IceHiveZ/internals/ConnectionConfig.h"

namespace bp = boost::python;

#include <boost/python/operators.hpp>
#include <boost/operators.hpp>
#include <boost/python/object.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/list.hpp>
#include <boost/python/wrapper.hpp>

#include <boost/foreach.hpp>
#include "icetray/python/dataclass_suite.hpp"

#include "helper.h"


///namespace containing specializations and casting-functions for pybindings
namespace pyConnectionConfig {
  //wrap pure virtual functions and get the overrides from derived classes
  struct ConnectionConfig_pyWrapper : public ConnectionConfig, public bp::wrapper<ConnectionConfig>
  { 
    bool CorrectlyConfigured() const 
      {return this->get_override("CorrectlyConfigured")();};
    ConnectionPtr BuildConnection (
      const HashedGeometryConstPtr& hashedGeo)
      {return this->get_override("BuildConnection")();};
  };
};


void register_ConnectionConfig()
{
  using namespace pyConnectionConfig;
  //=== class ConnectionConfig
  bp::class_<ConnectionConfig_pyWrapper, boost::noncopyable>("ConnectionConfig", bp::no_init)
    .def("correctlyConfigured",
      //bp::pure_virtual(&ConnectionConfig::CorrectlyConfigured),
      &ConnectionConfig::CorrectlyConfigured,
      "Is this correctly configured")
    .def("buildConnection", 
      bp::pure_virtual(&ConnectionConfig_pyWrapper::BuildConnection),
      bp::args("hasher"),
      "Build a connection from this configuration specifying hasher and distanceService")
    ;
  
  //=== class StaticConnectionConfig
  bp::class_<BoolConnectionConfig, bp::bases<ConnectionConfig>, BoolConnectionConfigPtr >("BoolConnectionConfig")
    .def_readwrite("connect_everything", &BoolConnectionConfig::connect_everything_);
  
  //=== class DeltaTimeConnectionConfig
  bp::class_<DeltaTimeConnectionConfig, bp::bases<ConnectionConfig>, DeltaTimeConnectionConfigPtr >("DeltaTimeConnectionConfig")
    .def_readwrite("time_difference_early", &DeltaTimeConnectionConfig::tresidual_early_)
    .def_readwrite("time_difference_late", &DeltaTimeConnectionConfig::tresidual_late_);
    
  //=== class DynamicConnectionConfig
  bp::class_<DynamicConnectionConfig, bp::bases<ConnectionConfig>, DynamicConnectionConfigPtr >("DynamicConnectionConfig")
    .def_readwrite("speed", &DynamicConnectionConfig::speed_)
    .def_readwrite("time_residual_early", &DynamicConnectionConfig::tresidual_early_)
    .def_readwrite("time_residual_late", &DynamicConnectionConfig::tresidual_late_);
  
  //=== class PhotonDiffusionConnectionConfig
  bp::class_<PhotonDiffusionConnectionConfig, bp::bases<ConnectionConfig>, PhotonDiffusionConnectionConfigPtr >("PhotonDiffusionConnectionConfig")
    .def_readwrite("lower_cont_quantile", &PhotonDiffusionConnectionConfig::lower_cont_quantile_)
    .def_readwrite("upper_cont_quantile", &PhotonDiffusionConnectionConfig::upper_cont_quantile_)
    .def_readwrite("time_residual_early", &PhotonDiffusionConnectionConfig::tresidual_early_)
    .def_readwrite("time_residual_late", &PhotonDiffusionConnectionConfig::tresidual_late_)
    .def_readwrite("min_pdfvalue", &PhotonDiffusionConnectionConfig::min_pdfvalue_);
};
