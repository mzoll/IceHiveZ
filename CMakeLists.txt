SET(PROJECT_NAME IceHiveZ)

#=== SERIALIZATION SUPPORT ==
#0 No Serialization support
#1 BOOST::SERIALIZATION support
#2 ICECUBE::SERIALIZATION support

if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/serialization")
  colormsg(GREEN "+-- ICECUBE::serialization support found")
  SET(SERIALIZATION_SUPPORT 2)
elseif(BOOST_FOUND)
  colormsg(GREEN "+-- BOOST::serialization support found")
  SET(SERIALIZATION_SUPPORT 1)
else()
  colormsg(YELLOW "+-- No serialization support found")
  set(SERIALIZATION_SUPPORT 0)
endif()

# SET(SERIALIZATION_SUPPORT 2) #//DANGER OVERIDE

if (SERIALIZATION_SUPPORT EQUAL 0)
  colormsg(YELLOW "+-- no support for object serialization configured")
  colormsg(RED    "+--   SERIALIZATION functionality will be disabled")
elseif (SERIALIZATION_SUPPORT EQUAL 1)
  colormsg(GREEN  "+-- BOOST::serialization support configured")
  colormsg(YELLOW  "+--   library might conflict with ICECUBE::serialization")
elseif (SERIALIZATION_SUPPORT EQUAL 2)
  colormsg(GREEN "+-- ICECUBE::serialization support configured")
else ()
  message(FATAL_ERROR "wrongly configured internal flag 'SERIALIZATION_SUPPORT'")
endif()

if(CMAKE_CXX_STANDARD)
  if (CMAKE_CXX_STANDARD EQUAL 11)
    colormsg(GREEN "+-- Supporting C++11")
  else()
    colormsg(GREEN "+-- Supporting C++${CMAKE_CXX_STANDARD}")
  endif()
else()
  if(SERIALIZATION_SUPPORT EQUAL 2)
    SET (CMAKE_CXX_STANDARD 11)
    colormsg(YELLOW "+-- OVERRIDE:local set 'CMAKE_CXX_STANDARD' to 11")
  else()
    SET (CMAKE_CXX_STANDARD 98)
    colormsg(YELLOW "+-- OVERRIDE:local set 'CMAKE_CXX_STANDARD' to 98")
  endif()
endif()

i3_project(${PROJECT_NAME}
  DOCS_DIR resources/docs
  PYTHON_DIR python
)

SET(LIB_${PROJECT_NAME}_HEADERFILES
#  public/IceHiveZ/__SERIALIZATION.h

  private/IceHiveZ/internals/ConfigInterfaces.h

  private/IceHiveZ/internals/Hive.h
  private/IceHiveZ/internals/Relation.h
  private/IceHiveZ/internals/RelationConfig.h
  private/IceHiveZ/internals/Connection.h
  private/IceHiveZ/internals/ConnectionConfig.h
  private/IceHiveZ/internals/Connector.h
  private/IceHiveZ/internals/Configurator.h

  private/IceHiveZ/algorithms/HiveSplitter.h
  private/IceHiveZ/algorithms/HiveTrigger.h
  private/IceHiveZ/algorithms/HiveCleaning.h
  private/IceHiveZ/i3modules/I3HiveSplitter.h
  private/IceHiveZ/i3modules/I3HiveCleaning.h
  private/IceHiveZ/i3modules/I3HiveClustering.h
  private/IceHiveZ/i3modules/I3GeoDeliver.h
)

SET(LIB_${PROJECT_NAME}_SOURCEFILES
  private/IceHiveZ/internals/ConfigInterfaces.cxx

  private/IceHiveZ/internals/Hive.cxx
  private/IceHiveZ/internals/Relation.cxx
  private/IceHiveZ/internals/RelationConfig.cxx
  private/IceHiveZ/internals/Connection.cxx
  private/IceHiveZ/internals/ConnectionConfig.cxx
  private/IceHiveZ/internals/Connector.cxx
  private/IceHiveZ/internals/Configurator.cxx

  private/IceHiveZ/algorithms/HiveSplitter.cxx
  private/IceHiveZ/algorithms/HiveTrigger.cxx
  private/IceHiveZ/algorithms/HiveCleaning.cxx
  private/IceHiveZ/i3modules/I3HiveSplitter.cxx
  private/IceHiveZ/i3modules/I3HiveCleaning.cxx
  private/IceHiveZ/i3modules/I3HiveClustering.cxx
  private/IceHiveZ/i3modules/I3GeoDeliver.cxx
)

SET(LIB_${PROJECT_NAME}_PYBINDINGS
  private/pybindings/module.cxx

  private/pybindings/Hive.cxx
  private/pybindings/Relation.cxx
  private/pybindings/RelationConfig.cxx
  private/pybindings/Connection.cxx
  private/pybindings/ConnectionConfig.cxx
  private/pybindings/Connector.cxx
  private/pybindings/Configurator.cxx
)

SET(${PROJECT_NAME}_UNITTESTS
  private/test/main.cxx
  private/test/TestHelpers.cxx
   
  private/test/HiveTest.cxx
  private/test/RelationTest.cxx
  private/test/ConnectionTest.cxx
  private/test/ConnectorTest.cxx
  private/test/HiveSplitterTest.cxx
  private/test/HiveCleaningTest.cxx
  
  private/test/HiveTriggerTest.cxx
  
  #private/test/ConnectorOldCompatLoad.cxx
)


SET(${PROJECT_NAME}_USEPROJECTS
  serialization
  icetray
  dataclasses
  simclasses
  phys-services
  dataio
  ToolZ
)

if (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/hitspool-reader")
  colormsg(GREEN "+-- hitspool-reader found, building IceHiveTrigger")
  SET(HITSPOOLREADER_FOUND 1)
  LIST(APPEND ${PROJECT_NAME}_USEPROJECTS
    hitspool-reader
  )
  LIST(APPEND LIB_${PROJECT_NAME}_HEADERFILES 
    private/IceHiveZ/modules/IceHiveTrigger.h
  )
  LIST(APPEND LIB_${PROJECT_NAME}_SOURCEFILES 
    private/IceHiveZ/modules/IceHiveTrigger.cxx
  )
  LIST(APPEND LIB_${PROJECT_NAME}_PYBINDINGS
    private/pybindings/IceHiveTrigger.cxx
  )
  LIST(APPEND ${PROJECT_NAME}_UNITTESTS
    private/test/IceHiveTriggerTest.cxx
  )
else ()
  colormsg(YELLOW "+-- hitspool-reader required to build IceHiveTrigger")
  colormsg(YELLOW "+--   pull the specific branch from SVN/sandbox/mzoll/hitspool-reader")
  SET(HITSPOOLREADER_FOUND 0)
endif()


SET(${PROJECT_NAME}_PREPROCESSORFLAGS
  -DSERIALIZATION_SUPPORT=${SERIALIZATION_SUPPORT}
  -DFUTURE=0
  #-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
  -DHITSPOOLREADER_FOUND=${HITSPOOLREADER_FOUND} 
)

SET(${PROJECT_NAME}_COMPILERINST
 -ftemplate-depth=512
 -std=c++11
) 

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  LIST(APPEND ${PROJECT_NAME}_COMPILERINST
    -std=gnu++11 
    -fext-numeric-literals
  )
endif()

add_definitions(${${PROJECT_NAME}_PREPROCESSORFLAGS} ${${PROJECT_NAME}_COMPILERINST} )

#add_definitions(-DSERIALIZATION_SUPPORT=${SERIALIZATION_SUPPORT} -DFUTURE=0 -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD} -ftemplate-depth=512 -std=gnu++11 -std=c++11 -fext-numeric-literals)

i3_add_library(${PROJECT_NAME}
  ${LIB_${PROJECT_NAME}_HEADERFILES}
  ${LIB_${PROJECT_NAME}_SOURCEFILES}
  USE_TOOLS boost python
  USE_PROJECTS ${${PROJECT_NAME}_USEPROJECTS}
)

i3_add_pybindings(${PROJECT_NAME}
  ${LIB_${PROJECT_NAME}_PYBINDINGS}
  USE_TOOLS boost python 
  USE_PROJECTS ${${PROJECT_NAME}_USEPROJECTS} ${PROJECT_NAME}
)

i3_test_executable(test
  ${${PROJECT_NAME}_UNITTESTS}
  USE_PROJECTS ${${PROJECT_NAME}_USEPROJECTS} ${PROJECT_NAME}
)

i3_test_scripts(
  resources/test/icehiveConfigTest.py
#   resources/test/i3hivesplitterTest.py
#   resources/test/i3hiveclusteringTest.py
#   resources/test/i3hivecleaningTest.py
)
