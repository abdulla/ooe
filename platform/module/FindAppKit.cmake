find_path( APPKIT_INCLUDE_DIRS AppKit/AppKit.h )
find_library( APPKIT_LIBRARIES AppKit )
mark_as_advanced( APPKIT_LIBRARIES APPKIT_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( AppKit DEFAULT_MSG APPKIT_LIBRARIES APPKIT_INCLUDE_DIRS )
