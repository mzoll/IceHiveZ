/**
 * \file IceHiveTrigger.cxx
 *
 * (c) 2013 the IceCube Collaboration
 *
 * \version $Revision: 144968 $
 * \date $Date: 2016-04-21 00:52:24 +0200 (tor, 21 apr 2016) $
 * \author mzoll <marcel.zoll@fysik.su.se>
 *
 * Pybindings to IceHiveTrigger
 */

#include "hitspool-reader/HitSpoolTrigger.h"
#include "IceHiveZ/modules/IceHiveTrigger.h"
#include "IceHiveZ/algorithms/HiveTrigger.h"

#include <boost/python.hpp>
#include <boost/python/list.hpp>

namespace bp = boost::python;
using namespace hitspooltrigger;
using namespace hivetrigger;

namespace pyIceHiveTrigger {};

void register_IceHiveTrigger() {

  boost::python::import("icecube.hitspool_reader"); //need to import dependent classes from here
  
  bp::class_<HiveTrigger_ParameterSet>(
    "HiveTrigger_ParameterSet")
    .def_readwrite("multiplicity", &HiveTrigger_ParameterSet::multiplicity)
    .def_readwrite("multiplicityTimeWindow", &HiveTrigger_ParameterSet::multiplicityTimeWindow)
    .def_readwrite("acceptTimeWindow", &HiveTrigger_ParameterSet::acceptTimeWindow)
    .def_readwrite("rejectTimeWindow", &HiveTrigger_ParameterSet::rejectTimeWindow)
    .def_readwrite("connectorBlock", &HiveTrigger_ParameterSet::connectorBlock)
    ;
  
  bp::class_<IceHiveTrigger, bp::bases<LaunchStreamTrigger> >("IceHiveTrigger", //, IceHiveTriggerPtr
    bp::init<const hivetrigger::HiveTrigger_ParameterSet, const size_t>
      (bp::args("hivetrigger_params","minEventSize"), "IceHiveTrigger needs to be provided with a fully intialized IceHive-ConnectorBlock and the minimal event size as parameters"));
}
