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
#include "IceHiveZ/internals/RelationConfig.h"

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
namespace pyRelationConfig {
  //========================= STRUCT RelationConfig_pyWrapper ====================
  struct RelationConfig_pyWrapper : public RelationConfig, public bp::wrapper<RelationConfig> {
    RelationPtr BuildRelation (
      const HashedGeometryConstPtr& hashedGeo) const
    {return this->get_override("BuildRelation")();};
  };
  
  //========================= STRUCT SimpleRelationConfig_pyWrapper ====================
  struct SimpleRelationConfig_pyWrapper {
    /// A wrapper around a python::object of signature 'bool __call__(const OMKey&, const OMKey&)',
    /// which calls and extracts the desired c++ types
    class wrap_bpo_func {
    private:
      const boost::python::object held_;
    public:
      wrap_bpo_func(boost::python::object held) :
        held_(held) 
      { //test if the python object is of the correct signature
        if (!PyCallable_Check(held_.ptr()))
          log_fatal("'ConnectFrom' and 'ConnectTo' parameter to HiveRelationConfig must be a callable object");
        else
          held(OMKey(1,1), OMKey(1,1)); //NOTE hobo call
        //if it has not failed until here, everything is fine
      };
      
      ///this is the call operator
      bool operator()(const OMKey& omkeyA, const OMKey& omkeyB) const {
        return boost::python::extract<bool>(held_(omkeyA, omkeyB));
      };
    };
    
    static
    SimpleRelationConfigPtr initCtor (
      const bp::object callobj) 
    {
      return boost::make_shared<SimpleRelationConfig>(wrap_bpo_func(callobj));
    };
  };

  //========================= STRUCT HiveRelationConfig_pyWrapper ====================
  struct HiveRelationConfig_pyWrapper {
    static
    void AddRingLimits (HiveRelationConfig& hc, const boost::python::list& list) {
      std::vector<HiveRelationConfig::LimitPair> vec;
      for (size_t i = 0; i<bp::len(list); i++) {
        boost::python::tuple tuple = bp::extract<boost::python::tuple>(list[i]);
          vec.push_back(HiveRelationConfig::LimitPair(bp::extract<double>(tuple[0]), bp::extract<double>(tuple[1])));
      }
      hc.ringLimits_=HiveRelationConfig::RingLimits(vec);
    };
    
    /// A wrapper around a python::object of signature 'bool __call__(const OMKey)',
    /// which calls and extracts the desired c++ types
    class wrap_bpo_func {
    private:
      const boost::python::object held_;
    public:
      wrap_bpo_func(boost::python::object held) :
        held_(held) 
      { //test if the python object is of the correct signature
        if (!PyCallable_Check(held_.ptr()))
          log_fatal("'ConnectFrom' and 'ConnectTo' parameter to HiveRelationConfig must be a callable object");
        else
          held(OMKey(1,1));
        //if it has not failed until here, everything is fine
      };
      
      ///this is the call operator
      bool operator()(const OMKey& omkey) const {
        return boost::python::extract<bool>(held_(omkey));
      };
    };

    static
    void AddConnectFrom(HiveRelationConfig& hrc, const boost::python::object& bpo) {
      hrc.connectFrom_ = wrap_bpo_func(bpo); // maybe boost::bind needs to used here
    };
    
    static
    void AddConnectTo(HiveRelationConfig& hrc, const boost::python::object& bpo) {
      hrc.connectTo_ = wrap_bpo_func(bpo); // maybe boost::bind needs to used here
    };
  };
};

//==================>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<===============
void register_RelationConfig()
{
  using namespace pyRelationConfig;
  //=== class ConnectionConfig
  bp::class_<RelationConfig_pyWrapper, boost::noncopyable>("RelationConfig")
    .def("buildRelation", 
      bp::pure_virtual(&RelationConfig_pyWrapper::BuildRelation),
      bp::args("hashedGeo"),
      "Build a Relation from this configuration specifying GCD,hasher and distanceService")
    ;
  bp::implicitly_convertible<RelationConfigPtr, RelationConfigConstPtr>();
    
  bp::class_<SimpleRelationConfig, bp::bases<RelationConfig>, SimpleRelationConfigPtr>("SimpleRelationConfig", bp::no_init)
    .def("__init__", 
      bp::make_constructor(&SimpleRelationConfig_pyWrapper::initCtor), 
      "A simple construction blue-print for a Relation from a simple python object that supports __call__(omkey, omkey)")
    ;
  bp::implicitly_convertible<SimpleRelationConfigPtr, SimpleRelationConfigConstPtr>();
    
  //=== class HiveRelationConfig
  bp::class_<HiveRelationConfig, bp::bases<RelationConfig>, HiveRelationConfigPtr>("HiveRelationConfig",
    bp::init<const hive::HiveTopologyConstPtr>(
      bp::args("hivetopo"),
      "Specify a HiveRelationConfig"))
    .def_readwrite("selfConnect", &HiveRelationConfig::selfconnect_)
    .def_readwrite("mutuallyConnect", &HiveRelationConfig::mutuallyconnect_)
    .def("AddRingLimits", //FIXME make this to a property!!!
      &HiveRelationConfig_pyWrapper::AddRingLimits,
      bp::args("ringLimitList"),
      "Add a list of Limit Pairs")
    .def("AddConnectFrom", //FIXME make this to a property!!! by getter/setter
      &HiveRelationConfig_pyWrapper::AddConnectFrom,
      bp::args("pyFunction"),
      "Specifies DOMs that can be connected from; a function that has signature: bool (const OMKey)")
    .def("AddConnectTo", //FIXME make this to a property!!! by getter/setter
      &HiveRelationConfig_pyWrapper::AddConnectTo,
      bp::args("pyFunction"),
      "Specifies DOMs that can be connected from; a function that has signature: bool (const OMKey)")
    ;
  bp::implicitly_convertible<HiveRelationConfigPtr, HiveRelationConfigConstPtr>();
};
