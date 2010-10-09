find_path( V4L_INCLUDE_DIRS libv4lconvert.h )
find_library( V4L_LIBRARIES v4lconvert )
mark_as_advanced( V4L_LIBRARIES V4L_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( V4L DEFAULT_MSG V4L_LIBRARIES V4L_INCLUDE_DIRS )
