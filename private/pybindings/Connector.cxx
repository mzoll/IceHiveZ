/**
 * \file Connector.cxx
 *
 * (c) 2013 the IceCube Collaboration
 *
 * $Id: IceHiveHelpers.cxx 129010 2015-02-12 17:21:06Z mzoll $
 * \version $Revision: 129010 $
 * \date $Date: 2015-02-12 18:21:06 +0100 (tor, 12 feb 2015) $
 * \author mzoll <marcel.zoll@fysik.su.se>
 *
 * Unit test to test the robustness of HitSorting as it is implemented in HiveSplitter/HitSorting.h
 */

#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "IceHiveZ/internals/Connector.h"
#include "ToolZ/Interfaces.h"

namespace bp = boost::python;

namespace pyConnector{
  struct ConnectorBlock_pyWrapper {
  #if SERIALIZATION_ENABLED
    static
    void
    WriteToFile (const ConnectorBlockPtr& cb, const std::string& filename)
      { fileinterfaces::WriteToFile(cb.get(), filename); };
    
    static 
    ConnectorBlockPtr
    ReadFromFile(const std::string& filename)
      { 
        ConnectorBlock* cb;
        fileinterfaces::ReadFromFile(cb, filename);
        return ConnectorBlockPtr(cb);
      };
  #endif //SERIALIZATION_ENABLED
  };
  
  bool Connected(
    const ConnectorBlockConstPtr& cb,
    const AbsHit& h1,
    const AbsHit& h2)
  {
    return cb->Connected(h1, h2);
  };
    
  void DiagnoseConnected(
    const ConnectorBlockConstPtr& cb,
    const AbsHit& h1,
    const AbsHit& h2)
  {
    return cb->DiagnoseConnected(h1, h2);
  };
  
};


void register_Connector() {
  using namespace pyConnector;
  
  bp::class_<Connector, ConnectorPtr>("Connector", 
    bp::init<const std::string&,
              const HashedGeometryConstPtr&,
              const ConnectionPtr&, 
              const RelationPtr&>(bp::args("name","hasher","connecton","relation"), 
                                  "A class evaluating the connection between hits"))
    .def_readonly("name", 
      &Connector::GetName)
    .def_readonly("hasher", &Connector::GetHashService)
    .def_readonly("connection",
      &Connector::GetConnection)
    .def_readonly("relation",
      &Connector::GetRelation);
    //.def(Connected) FIXME
  
  bp::class_<ConnectorBlock, ConnectorBlockPtr>("ConnectorBlock",
    bp::init<const HashedGeometryConstPtr&>(bp::args("hasher"), "class holding a multitude of Connectors"))
    .def("getConnector",
         &ConnectorBlock::GetConnector,
         bp::args("index"),
         "Get a specific Connector: -1 for the cumulative Connector, (int) the Connector at this position in the list")
    .def("addConnector", 
         &ConnectorBlock::AddConnector,
         bp::args("connector"),
         "Add a ConnectonService to the Block")
    #if SERIALIZATION_ENABLED
    .def("to_file",
         &ConnectorBlock_pyWrapper::WriteToFile,
         bp::args("filename"),
         "Write to File")
    .def("from_file",
         &ConnectorBlock_pyWrapper::ReadFromFile,
         bp::args("filename"),
         "Read a ConnectorBlock from file")
    .staticmethod("from_file")
    #endif //SERIALIZATION_ENABLED
    .def("connected",
         &pyConnector::Connected,
         bp::args("abshit","abshit"),
        "probe the connection of two abshits")
    .def("diagnoseConnected",
       &pyConnector::DiagnoseConnected,
         bp::args("abshit","abshit"),
        "diagnose the connection of two abshits")
    
    ;
    
  bp::implicitly_convertible<ConnectorBlockPtr, ConnectorBlockConstPtr>();
}; //register_Connector

