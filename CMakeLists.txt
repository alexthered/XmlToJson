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
file (GLOB TOP_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
     )


#add XML and Json library
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/tinyXml )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/json)

#include source file in all folder
set(PROJECT_SRCS
    ${TOP_SRCS}
    ${TINYXML_SRCS}
    ${JSON_SRCS}
    )
    
# Create output folder with build type (release/debug)
add_executable(${PROJECT_NAME} ${PROJECT_SRCS})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})
