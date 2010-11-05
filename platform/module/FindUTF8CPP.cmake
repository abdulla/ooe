find_path( UTF8CPP_INCLUDE_DIRS utf8.h PATH_SUFFIXES utf8cpp )
mark_as_advanced( UTF8CPP_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( UTF8CPP DEFAULT_MSG UTF8CPP_INCLUDE_DIRS )
