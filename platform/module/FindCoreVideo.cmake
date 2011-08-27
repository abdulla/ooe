find_path( COREVIDEO_INCLUDE_DIRS CoreVideo/CoreVideo.h )
find_library( COREVIDEO_LIBRARIES CoreVideo )
mark_as_advanced( COREVIDEO_LIBRARIES COREVIDEO_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( CoreVideo DEFAULT_MSG COREVIDEO_LIBRARIES
    COREVIDEO_INCLUDE_DIRS )
