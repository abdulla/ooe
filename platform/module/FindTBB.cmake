find_path( TBB_INCLUDE_DIRS tbb/tbb.h )
find_library( TBB_LIBRARY tbb )
find_library( TBBMALLOC_LIBRARY tbbmalloc )
set( TBB_LIBRARIES ${TBB_LIBRARY} ${TBBMALLOC_LIBRARY} )
mark_as_advanced( TBB_LIBRARIES TBB_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( TBB DEFAULT_MSG TBB_LIBRARIES TBB_INCLUDE_DIRS )
