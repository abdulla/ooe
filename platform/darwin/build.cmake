### flags ##########################################################################################
set( CMAKE_CXX_COMPILER g++-4 )
set( CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,@executable_path/../lib" )
set( CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup -dead_strip" )
set( CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup -dead_strip" )

### packages #######################################################################################
find_package( AppKit REQUIRED )
include_directories( ${APPKIT_INCLUDE_DIRS} )
set( EXECUTABLE_LIBRARIES ${APPKIT_LIBRARIES} )

find_package( QTKit REQUIRED )
include_directories( ${QTKIT_INCLUDE_DIR} )
set( VISUAL_LIBRARIES ${QTKIT_LIBRARIES} )
