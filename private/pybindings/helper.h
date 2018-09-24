/**
 * copyright  (C) 2012
 * the IceCube Collaboration
 * $Id: $
 *
 * @file helper.cxx
 * @date $Date: 2012-12-20$
 * @author mzoll <marcel.zoll@fysik.su.se>
 * 
 * just some conversions from bp::list to various c++ std-container types
 */

#ifndef PYHELPER_H
#define PYHELPER_H

#include <vector>
#include <list>
#include <set>
#include <string>

#include <boost/python/operators.hpp>
#include <boost/operators.hpp>
#include <boost/python/list.hpp>
// #include <boost/python/set.hpp>

namespace bp=boost::python;

// Converts a python list to a C++ vector
template <class T>
std::vector<T> PyListToStdVector(boost::python::list pylist) {
    std::vector<T> vector;
    uint64_t n_items = bp::len(pylist);
    for (uint64_t i = 0; i<n_items; i++) {
        vector.push_back(bp::extract<T>(pylist[i]));
    }
    return vector;
};
  
// Converts a python list to a C++ list
template <class T>
std::list<T> PyListToStdList(boost::python::list pylist) {
    std::list<T> list;
    uint64_t n_items = bp::len(pylist);
    for (uint64_t i = 0; i<n_items; i++) {
        list.push_back(bp::extract<T>(pylist[i]));
    }
    return list;
};

// Converts a C++ list to a python list
template <class T>
boost::python::list StdListToPyList(std::list<T> list) {
    boost::python::list pylist;
    BOOST_FOREACH(const T& t, list)
      pylist.append(t);
    return pylist;
};

// Converts a C++ vector to a python list
template <class T>
boost::python::list StdVectorToPyList(std::list<T> list) {
    boost::python::list pylist;
    BOOST_FOREACH(const T& t, list)
      pylist.append(t);
    return pylist;
};

// Converts a C++ list to a python list
// template <class T>
// boost::python::list StdVectorToPyList(std::list<T> list) {
//     boost::python::set pyset;
//     BOOST_FOREACH(const T& t, list)
//       pyset.insert(t);
//     return pyset;
// };


#endif //PYHELPER_H
