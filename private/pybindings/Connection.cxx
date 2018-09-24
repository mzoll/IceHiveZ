/**
 * \file Connection.cxx
 *
 * (c) 2013 the IceCube Collaboration
 *
 * $Id: IceHiveHelpers.cxx 129010 2015-02-12 17:21:06Z mzoll $
 * \version $Revision: 129010 $
 * \date $Date: 2015-02-12 18:21:06 +0100 (tor, 12 feb 2015) $
 * \author mzoll <marcel.zoll@fysik.su.se>
 *
 */

#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "IceHiveZ/internals/Connection.h"

namespace bp = boost::python;

void register_Connection() {
  //=== class Connection  
  bp::class_<Connection, boost::noncopyable, ConnectionPtr>("Connection",  bp::no_init)
    .def_readonly("hasher", &Connection::GetHasher);
  
  //=== class StaticConnection
  bp::class_<BoolConnection, bp::bases<Connection>, BoolConnectionPtr >("BoolConnection", 
    bp::init<const HashedGeometryConstPtr&>(
      bp::args("hashedGeo"),
      "A Connection that is just a simple bool decision"))
    .def_readwrite("connect_everything", &BoolConnection::connect_everything_);
  
  //=== class DeltaTimeConnection
  bp::class_<DeltaTimeConnection, bp::bases<Connection>, DeltaTimeConnectionPtr >("DeltaTimeConnection",
    bp::init<const HashedGeometryConstPtr&>(
      bp::args("hashedGeo"),
      "A Connection that checks a positive and negative time residual between hits"))
    .def_readwrite("time_difference_early", &DeltaTimeConnection::tresidual_early_)
    .def_readwrite("time_difference_late", &DeltaTimeConnection::tresidual_late_);
    
  //=== class DynamicConnection
  bp::class_<DynamicConnection, bp::bases<Connection>, DynamicConnectionPtr >("DynamicConnection",
    bp::init<const HashedGeometryConstPtr&>(
      bp::args("hashedGeo"),
      "A Connection that computes the time residual for a travelling messenger between hits"))
    .def_readwrite("speed", &DynamicConnection::speed_)
    .def_readwrite("time_residual_early", &DynamicConnection::tresidual_early_)
    .def_readwrite("time_residual_late", &DynamicConnection::tresidual_late_);
  
  //=== class PhotonDiffusionConnection
  bp::class_<PhotonDiffusionConnection, bp::bases<Connection>, PhotonDiffusionConnectionPtr >("PhotonDiffusionConnection",
    bp::init<const HashedGeometryConstPtr&>(
      bp::args("hashedGeo"),
      "A Connection that probes the propagation of photons by the PandelPDF between hits"))
    .def_readwrite("lower_cont_quantile", &PhotonDiffusionConnection::lower_cont_quantile_)
    .def_readwrite("upper_cont_quantile", &PhotonDiffusionConnection::upper_cont_quantile_)
    .def_readwrite("time_residual_early", &PhotonDiffusionConnection::tresidual_early_)
    .def_readwrite("time_residual_late", &PhotonDiffusionConnection::tresidual_late_);
}; //register_Connection

