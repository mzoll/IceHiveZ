/**
 * copyright  (C) 2012
 * the IceCube Collaboration
 * $Id: $
 *
 * @file Configurator.cxx
 * @date $Date: 2012-12-20$
 * @author mzoll <marcel.zoll@fysik.su.se>
 */

#include <string>

#include "icetray/python/stream_to_string.hpp"
#include "dataclasses/ostream_overloads.hpp"
#include "IceHiveZ/internals/Configurator.h"

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
namespace pyConfigurator {
  struct ConfiguratorBlock_pyWrapper : public ConfiguratorBlock, public bp::wrapper<ConfiguratorBlock> {
    ConnectorBlock
    BuildConnectorBlock__OMGeoMap (
      const ConfiguratorBlock& confb,
      const I3OMGeoMap& omgeo)
    { return confb.BuildConnectorBlock(omgeo); };
  
    class wrap_bpo_func {
    private:
      const boost::python::object held_;
    public:
      wrap_bpo_func(boost::python::object held) :
        held_(held) 
      { //test if the python object is of the correct signature
        if (!PyCallable_Check(held_.ptr()))
          log_fatal("object must be a callable by signature: bool __call__(omkey)");
      };
      
      ///this is the call operator
      bool operator()(const OMKey& omkey) const {
        return boost::python::extract<bool>(held_(omkey));
      };
    };
    
    static
    void SetOMKeys(ConfiguratorBlock& cb, const boost::python::object& bpo) {
      cb.hashOMKeys_ = wrap_bpo_func(bpo); // maybe boost::bind needs to used here
    };
  };
};

void register_Configurator()
{
  using namespace pyConfigurator;
  
  //=== class Configurator
  bp::class_<Configurator, ConfiguratorPtr>("Configurator",
    bp::init<const std::string&>(
      bp::args("name"),
      "Specify a Configurator"))
    .def("AddConnectionConfig",
      &Configurator::AddConnectionConfig,
      bp::args("connectionConfig"),
      "Add a connectionConfig for this")
    .def("AddRelationConfig",
      &Configurator::AddRelationConfig,
      bp::args("relationConfig"),
      "Add a relationConfig for this")
    ;
  bp::implicitly_convertible<ConfiguratorPtr, ConfiguratorConstPtr>();
  
  //=== class ConfiguratorBlock
  bp::class_<ConfiguratorBlock, ConfiguratorBlockPtr>("ConfiguratorBlock",
    bp::init<>("Add Configurators to this block"))
    .def("add",  
      &ConfiguratorBlock::AddConfigurator,
      (bp::arg("hiveconfig")),
      "Add a Configurator to the Block")
    .def("takeOMKeys",
      &pyConfigurator::ConfiguratorBlock_pyWrapper::SetOMKeys,
      (bp::args("takeOMKeys")),
      "A function, with signature: bool __call__(omkey)")
    .def("buildConnectorBlock",
      &ConfiguratorBlock::BuildConnectorBlock,
      bp::args("omgeo"),
      "Build a ConnectorBlock from this configuration")
    ;
  bp::implicitly_convertible<ConfiguratorBlockPtr, ConfiguratorBlockConstPtr>();  
};
