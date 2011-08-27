### out-of-source ##################################################################################
get_filename_component( PARENT_DIR ${PROJECT_BINARY_DIR} PATH )

if( PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR OR PROJECT_SOURCE_DIR STREQUAL PARENT_DIR )
    message( FATAL_ERROR "CMake must be run outside of the source directory." )
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/CMakeCache.txt OR EXISTS ${CMAKE_SOURCE_DIR}/CMakeFiles )
    message( FATAL_ERROR
        "Source tree is polluted with build files, please remove CMakeCache.txt and CMakeFiles." )
endif()

### flags ##########################################################################################
option( BUILD_SHARED_LIBS "Cause libraries to be built as shared or static." ON )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY bin )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE DEBUG )
else()
    string( TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE )
endif()

if( CMAKE_BUILD_TYPE STREQUAL "DEBUG" )
    unset( CMAKE_C_FLAGS_DEBUG CACHE )
    unset( CMAKE_CXX_FLAGS_DEBUG CACHE )
    add_definitions( -O0 -g2 -fno-inline -fstack-protector-all -D_FORTIFY_SOURCE=2 )
elseif( CMAKE_BUILD_TYPE STREQUAL "RELEASE" )
    unset( CMAKE_C_FLAGS_RELEASE CACHE )
    unset( CMAKE_CXX_FLAGS_RELEASE CACHE )
    add_definitions( -O3 -g0 -fomit-frame-pointer -ffast-math -ftracer -fweb -fvisibility=hidden
        -fvisibility-inlines-hidden )
else()
    message( FATAL_ERROR
        "Project ${PROJECT_NAME} does not support build type ${CMAKE_BUILD_TYPE}." )
endif()

add_definitions( -std=c++98 -pedantic-errors -pipe -fstrict-aliasing -funit-at-a-time
    -fuse-cxa-atexit )

add_definitions( -Wall -Wcast-align -Werror -Wextra -Wfatal-errors -Wfloat-equal -Wformat=2
    -Wmissing-include-dirs -Wno-long-long -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith
    -Wreorder -Wshadow -Wstrict-aliasing -Wswitch-default -Wundef -Wwrite-strings )

### platform #######################################################################################
if( APPLE )
    set( PLATFORM darwin )
elseif( UNIX )
    set( PLATFORM posix )
endif()

include( platform/${PLATFORM}/build.cmake )
include_directories( platform/${PLATFORM} )

### functions ######################################################################################
macro( ooe_glob NAME )
    foreach( PATH ${ARGN} )
        file( GLOB SOURCES ${PATH}/*.cpp )
        list( APPEND CPP_SOURCES ${SOURCES} )
    endforeach()

    foreach( PATH ${ARGN} )
        file( GLOB SOURCES platform/${PLATFORM}/${PATH}/*.cpp )
        list( APPEND PLATFORM_CPP_SOURCES ${SOURCES} )
    endforeach()

    foreach( PATH ${ARGN} )
        file( GLOB SOURCES platform/${PLATFORM}/${PATH}/*.mm )
        list( APPEND PLATFORM_MM_SOURCES ${SOURCES} )
    endforeach()

    set_source_files_properties( ${PLATFORM_MM_SOURCES} PROPERTIES LANGUAGE C )
    set( ${NAME} ${CPP_SOURCES} ${PLATFORM_CPP_SOURCES} ${PLATFORM_MM_SOURCES} )
endmacro()

function( ooe_executable NAME )
    ooe_glob( SOURCES ${ARGN} )
    add_executable( ${NAME} ${SOURCES} )
    set_target_properties(
        ${NAME} PROPERTIES BUILD_WITH_INSTALL_RPATH ON INSTALL_RPATH \${ORIGIN}/../lib )
endfunction()

function( ooe_library NAME )
    ooe_glob( SOURCES ${ARGN} )
    add_library( ${NAME} ${SOURCES} )
    set_target_properties( ${NAME} PROPERTIES BUILD_WITH_INSTALL_RPATH ON INSTALL_NAME_DIR @rpath )
endfunction()

function( ooe_module NAME )
    ooe_glob( SOURCES ${ARGN} )
    add_library( ${NAME} MODULE ${SOURCES} )
endfunction()

function( ooe_static NAME )
    ooe_glob( SOURCES ${ARGN} )
    add_library( ${NAME} STATIC ${SOURCES} )
endfunction()

function( ooe_files SOURCE TARGET )
    file( GLOB_RECURSE FILES RELATIVE ${SOURCE} ${SOURCE}/* )

    foreach( FILE ${FILES} )
        configure_file( ${SOURCE}/${FILE} ${TARGET}/${FILE} COPYONLY )
    endforeach()
endfunction()

function( ooe_directories DIRS )
    foreach( DIR ${DIRS} )
        add_definitions( -isystem ${DIR} )
    endforeach()
endfunction()
