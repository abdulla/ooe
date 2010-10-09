find_path( OPENJPEG_INCLUDE_DIRS openjpeg.h )
find_library( OPENJPEG_LIBRARIES openjpeg )
mark_as_advanced( OPENJPEG_LIBRARIES OPENJPEG_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( OpenJPEG DEFAULT_MSG OPENJPEG_LIBRARIES OPENJPEG_INCLUDE_DIRS )
