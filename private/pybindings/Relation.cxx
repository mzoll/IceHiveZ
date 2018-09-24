/**
 * \file Relation.cxx
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

#include "IceHiveZ/internals/Relation.h"
#include "ToolZ/Interfaces.h"

namespace bp = boost::python;

namespace pyRelation {
  /// A wrapper around a python::object of signature 'bool __call__(const OMKey)',
  /// which calls and extracts the desired c++ types
  class wrap_bpo_call_bool_OMKey_OMKey {
  private:
    const boost::python::object held_;
  public:
    wrap_bpo_call_bool_OMKey_OMKey(boost::python::object held)
    : held_(held) 
    {
      //test if the python object is of the correct signature
      if (!PyCallable_Check(held_.ptr()))
        log_fatal("function argument needs to be be a callable object of signature 'bool __call__(OMKey, OMKey)'");
      else
        held(OMKey(1,1), OMKey(1,1));
      //if it has not failed until here, everything is fine
    };
    
    ///this is the call operator
    bool operator()(const OMKey& omkey1, const OMKey& omkey2) const {
      return boost::python::extract<bool>(held_(omkey1, omkey2));
    };
  };
  
  struct Relation_pyWrapper : public Relation, public bp::wrapper<Relation> {
    static
    RelationPtr
    pyRelation_pyCtor( 
      const CompactOMKeyHashServiceConstPtr& hasher,
      const bp::object& bpo)
    {return boost::make_shared<Relation>(hasher, wrap_bpo_call_bool_OMKey_OMKey(bpo));};
    
    static
    void 
    pyRelation_PredicateRelated(Relation& r, boost::python::object bpo) {
      r.PredicateRelated(wrap_bpo_call_bool_OMKey_OMKey(bpo));
    };
  };
};


void register_Relation() {
  using namespace pyRelation;
  
  bp::class_<Relation, RelationPtr>("Relation", bp::no_init)
    .def("__init__",
         bp::make_constructor(&Relation_pyWrapper::pyRelation_pyCtor),
//          bp::args("hasher","predicate"),
         "Construct and set all entries by predicate")
    .def(bp::init<
      const CompactOMKeyHashServiceConstPtr&,
      const bool>(
      bp::args("hasher","setall"),
      "Construct and set all entries of the Relation"))
    .def("setAll",
      &Relation::SetAllRelated,
      "Set all entries to be related")
    .def("setNone",
      &Relation::SetNoneRelated,
      "Set none of the entries to be related")
    .def("predicate",
      &Relation_pyWrapper::pyRelation_PredicateRelated,
      bp::args("predicate_function"),
      "set the relations, by a predicate function providing a call(OMKey, OMKey) operator")
    .def("areRelated",
         (bool (Relation::*)(const OMKey&, const OMKey&) const)&Relation::AreRelated, 
         bp::args("omkey", "omkey"),
         "Are these DOMs related (onesided)")
    .def("setRelated",
         (void (Relation::*)(const OMKey&, const OMKey&, const bool))&Relation::SetRelated,
         bp::args("omkey", "omkey", "bool"),
         "Set the relation for these DOMs (onesided)")
    ;
  bp::implicitly_convertible<RelationPtr, RelationConstPtr>();  
}; //register_Relation

