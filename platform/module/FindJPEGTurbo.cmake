find_path( JPEGTURBO_INCLUDE_DIRS jpeglib.h PATHS /sw/lib/libjpeg-turbo/include )
find_library( JPEGTURBO_LIBRARIES jpeg PATHS /sw/lib/libjpeg-turbo/lib )
mark_as_advanced( JPEGTURBO_LIBRARIES JPEGTURBO_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( JPEGTurbo DEFAULT_MSG JPEGTURBO_LIBRARIES
    JPEGTURBO_INCLUDE_DIRS )
