find_path( PYTHON_INCLUDE_DIRS python3.1/Python.h )
find_library( PYTHON_LIBRARIES python3.1 PATH_SUFFIXES python3.1/config )
mark_as_advanced( PYTHON_LIBRARIES PYTHON_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Python DEFAULT_MSG PYTHON_LIBRARIES PYTHON_INCLUDE_DIRS )
