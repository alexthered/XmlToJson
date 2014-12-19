#
# Project properties 
#
  
project(XmlToJson)
cmake_minimum_required( VERSION 2.8 )

# Always include the source and build directories in the include path
# to save doing so manually in every subdirectory.
set( CMAKE_INCLUDE_CURRENT_DIR ON )

#Use QT4
find_package(Qt4)
set( QT_USE_QTGUI FALSE )  #use GUI
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

#include source file in top folder
file (GLOB XML_TO_JSON_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/xmljsonparser.h
     ${CMAKE_CURRENT_SOURCE_DIR}/xmljsonparser.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/xmltojson.cpp
     )

file (GLOB JSON_TO_XML_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/xmljsonparser.h
     ${CMAKE_CURRENT_SOURCE_DIR}/xmljsonparser.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/jsontoxml.cpp
     )

#add XML and Json library
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/tinyXml )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/json)
    
# Set location for binary files
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin )

# Create output folder
add_executable(XmlToJson ${XML_TO_JSON_SRCS} ${TINYXML_SRCS} ${JSON_SRCS})
target_link_libraries(XmlToJson ${QT_LIBRARIES})

add_executable(JsonToXml ${JSON_TO_XML_SRCS} ${TINYXML_SRCS} ${JSON_SRCS})
target_link_libraries(JsonToXml ${QT_LIBRARIES})
