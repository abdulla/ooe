find_path( SQUISH_INCLUDE_DIRS squish.h )
find_library( SQUISH_LIBRARIES squish )
mark_as_advanced( SQUISH_LIBRARIES SQUISH_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Squish DEFAULT_MSG SQUISH_LIBRARIES SQUISH_INCLUDE_DIRS )
