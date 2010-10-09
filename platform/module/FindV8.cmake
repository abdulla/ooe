find_path( V8_INCLUDE_DIRS v8.h )
find_library( V8_LIBRARIES v8 )
mark_as_advanced( V8_LIBRARIES V8_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( V8 DEFAULT_MSG V8_LIBRARIES V8_INCLUDE_DIRS )
