### flags ##########################################################################################
set( CMAKE_MODULE_LINKER_FLAGS "-Wl,--as-needed" )
set( CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed" )

### packages #######################################################################################
find_library( DL_LIBRARY dl )
find_library( RT_LIBRARY rt )
set( EXECUTABLE_LIBRARIES ${DL_LIBRARY} ${RT_LIBRARY} )

find_package( V4L REQUIRED )
include_directories( ${V4L_INCLUDE_DIR} )

find_package( X11 REQUIRED )
include_directories( ${X11_Xrandr_INCLUDE_PATH} )
set( VISUAL_LIBRARIES ${V4L_LIBRARIES} ${X11_Xrandr_LIB} )
