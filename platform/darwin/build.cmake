### flags ##########################################################################################
set( CMAKE_C_COMPILER gcc-4.2 )
set( CMAKE_CXX_COMPILER g++-fsf-4.5 )
set( CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,@executable_path/../lib" )
set( CMAKE_MODULE_LINKER_FLAGS "-Wl,-dead_strip" )
set( CMAKE_SHARED_LINKER_FLAGS "-Wl,-dead_strip" )

### packages #######################################################################################
find_package( AppKit REQUIRED )
include_directories( ${APPKIT_INCLUDE_DIRS} )
set( EXECUTABLE_LIBRARIES ${APPKIT_LIBRARIES} )

find_package( CoreVideo REQUIRED )
find_package( QTKit REQUIRED )
include_directories( ${COREVIDEO_INCLUDE_DIR} ${QTKIT_INCLUDE_DIR} )
set( VISUAL_LIBRARIES ${COREVIDEO_LIBRARIES} ${QTKIT_LIBRARIES} )
