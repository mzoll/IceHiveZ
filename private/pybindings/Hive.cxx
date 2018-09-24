/**
 * copyright  (C) 2012
 * the IceCube Collaboration
 * $Id: $
 *
 * @file Hive.cxx
 * @date $Date: 2012-12-20$
 * @author mzoll <marcel.zoll@fysik.su.se>
 */

#include <string>

#include "icetray/python/stream_to_string.hpp"
#include "icetray/python/dataclass_suite.hpp"
#include "dataclasses/ostream_overloads.hpp"

#include "IceHiveZ/internals/Hive.h"
#include "ToolZ/Interfaces.h"
#include "IceHiveZ/internals/ConfigInterfaces.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace bp = boost::python;

using namespace hive;

///namespace containing specializations and casting-functions for pybindings
namespace pyHive {

  struct Hive_pyWrapper {
    #if SERIALIZATION_ENABLED
    void WriteToFile(const HiveTopologyPtr& hivetopo, const std::string &filename)
      { fileinterfaces::WriteToFile<HiveTopology>(hivetopo.get(), filename); };
    
    static
    HiveTopologyPtr
    ReadFromFile(const std::string &filename)
      {
        HiveTopology* hivetopo;
        fileinterfaces::ReadFromFile<hive::HiveTopology>(hivetopo, filename);
        return hive::HiveTopologyPtr(hivetopo);
      };
    #endif //SERIALIZATION_ENABLED

    void
    WriteToConfigFile(const HiveTopology& ht, const std::string &filename)
      { configfileinterfaces::WriteToConfigFile<HiveTopology>(ht, filename); };
      
    static 
    HiveTopologyPtr
    ReadFromConfigFile(const std::string &filename)
      { 
        HiveTopology* ht;
        configfileinterfaces::ReadFromConfigFile<HiveTopology>(ht, filename);
        return HiveTopologyPtr(ht);
      };
  };
}

void register_Hive() {
  using namespace pyHive;
  
  bp::class_<HiveTopology, HiveTopologyPtr>("HiveTopology",
    bp::init<>("Create empty HiveTopology"))
    #if SERIALIZATION_ENABLED
    .def("to_file",
         &Hive_pyWrapper::WriteToFile,
         bp::args("filename"), 
         "write/serialize a HiveTopology to a file")
    .def("from_file", //DANGER DISABLED
         &Hive_pyWrapper::ReadFromFile,
         bp::args("filename"),
         "read a serialized HiveTopology from file")  
    .staticmethod("from_file")
    #endif //SERIALIZATION_ENABLED
    .def("to_config", 
         &Hive_pyWrapper::WriteToConfigFile, 
         bp::args("filename"), 
         "write a HiveTopology to an plain text configuration file")
    .def("from_config",
         &Hive_pyWrapper::ReadFromConfigFile,
         bp::args("filename"),
         "read a configuration file to obtain the HiveTopology")
    .staticmethod("from_config")
    ;
    
  bp::implicitly_convertible<HiveTopologyPtr, HiveTopologyConstPtr>();
}
