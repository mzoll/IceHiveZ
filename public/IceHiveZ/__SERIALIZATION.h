/**
 * The shitty icecube::serialization breaks the serialization of shared pointers
 * Make a switch so we can choose between icecube::serialization and boost::serialization
 */

#ifndef SERIALIZATION_SWITCH_H
#define SERIALIZATION_SWITCH_H

#if SERIALIZATION_SUPPORT == 0
  #define SERIALIZATION_ENABLED 0
  #define SERIALIZATION_SUPPORT_NONE
#elif SERIALIZATION_SUPPORT == 1
  #define SERIALIZATION_ENABLED 1
  #define SERIALIZATION_SUPPORT_BOOST
#elif SERIALIZATION_SUPPORT == 2
  #define SERIALIZATION_ENABLED 1
  #define SERIALIZATION_SUPPORT_ICECUBE
#endif  
  

#if SERIALIZATION_SUPPORT == 1
// INCLUDE_SERIALIZATION_HEADERS 
  #include "icetray/serialization.h"
  #include <boost/serialization/serialization.hpp> //in icetray/serialization.h
  #include <boost/serialization/access.hpp>
  #include <boost/serialization/split_member.hpp> 
  #include <boost/serialization/shared_ptr.hpp>
//   #include <boost/serialization/base_object.hpp> //in icetray/serialization.h
//   #include <boost/serialization/string.hpp> //in icetray/serialization.h
//   #include <boost/serialization/vector.hpp> //in icetray/serialization.h
//   #include <boost/serialization/map.hpp> //in icetray/serialization.h
  #include <boost/serialization/list.hpp>
  #include <boost/serialization/set.hpp>
  #include <boost/serialization/bitset.hpp>
//   #include <boost/serialization/version.hpp> //in icetray/serialization.h
//   #include <boost/serialization/export.hpp> //in icetray/serialization.h
#elif SERIALIZATION_SUPPORT == 2
  #include "icetray/serialization.h"
// INCLUDE_SERIALIZATION_HEADERS {
  #include "serialization/serialization.hpp"
  #include "serialization/access.hpp"
  #include "serialization/split_member.hpp"
  #include "serialization/shared_ptr.hpp"
//   #include "serialization/base_object.hpp" //in icetray/serialization.h
//   #include "serialization/vector.hpp" //in icetray/serialization.h
//   #include "serialization/map.hpp" //in icetray/serialization.h
//   #include "serialization/string.hpp" //in icetray/serialization.h
  #include "serialization/list.hpp"
  #include "serialization/set.hpp"
  #include "serialization/bitset.hpp"
//   #include "serialization/version.hpp" //in icetray/serialization.h
//   #include "serialization/export.hpp" //in icetray/serialization.h
#endif
  
//==================================
//define some namespace shorthands
#if SERIALIZATION_SUPPORT == 1
  #define SERIALIZATION_NS_BASE boost
  #define SERIALIZATION_NS boost::serialization 
#elif SERIALIZATION_SUPPORT==2
  #define SERIALIZATION_NS_BASE icecube
  #define SERIALIZATION_NS icecube::serialization
#endif

//redefine BOOST/I3 macros
#if SERIALIZATION_SUPPORT == 1
  #define SERIALIZATION_CLASS_VERSION(c, v) BOOST_CLASS_VERSION( c, v );
  #define SERIALIZATION_BASE_OBJECT_NVP(c) BOOST_SERIALIZATION_BASE_OBJECT_NVP( c );
  #define SERIALIZATION_NVP(c) BOOST_SERIALIZATION_NVP( c );
  #define SERIALIZATION_ASSUME_ABSTRACT(c) BOOST_SERIALIZATION_ASSUME_ABSTRACT( c )
#elif SERIALIZATION_SUPPORT == 2
  #define SERIALIZATION_CLASS_VERSION(c, v) I3_CLASS_VERSION( c , v );
  #define SERIALIZATION_BASE_OBJECT_NVP(c) I3_SERIALIZATION_BASE_OBJECT_NVP( c );
  #define SERIALIZATION_NVP(c) I3_SERIALIZATION_NVP( c );
  #define SERIALIZATION_ASSUME_ABSTRACT(c) I3_SERIALIZATION_ASSUME_ABSTRACT( c )
#endif

#endif //SERIALIZATION_SWITCH_H
