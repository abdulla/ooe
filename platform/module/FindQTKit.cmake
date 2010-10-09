find_path( QTKIT_INCLUDE_DIRS QTKit/QTKit.h )
find_library( QTKIT_LIBRARIES QTKit )
mark_as_advanced( QTKIT_LIBRARIES QTKIT_INCLUDE_DIRS )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( QTKit DEFAULT_MSG QTKIT_LIBRARIES QTKIT_INCLUDE_DIRS )
