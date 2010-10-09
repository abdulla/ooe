### out-of-source ##################################################################################
if( PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR )
    message( FATAL_ERROR "CMake must be run outside of the source directory." )
endif()

### flags ##########################################################################################
option( BUILD_SHARED_LIBS "Cause libraries to be built as shared or static." ON )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY bin )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE DEBUG )
endif()

string( TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE )

if( BUILD_TYPE STREQUAL "DEBUG" )
    add_definitions( -O0 -g2 -fno-inline -fstack-protector-all -D_FORTIFY_SOURCE=2 )
else()
    add_definitions( -O3 -g0 -fomit-frame-pointer -ffast-math -ftracer -fweb -fvisibility=hidden
        -fvisibility-inlines-hidden )
endif()

add_definitions( -pipe -ansi -pedantic-errors -fno-enforce-eh-specs -fuse-cxa-atexit
    -funit-at-a-time -fstrict-aliasing -mfpmath=sse )

add_definitions( -Wall -Wextra -Werror -Wshadow -Wfloat-equal -Wnon-virtual-dtor -Wcast-align
    -Woverloaded-virtual -Wreorder -Wpointer-arith -Wwrite-strings -Wno-long-long -Wformat=2
    -Wstrict-aliasing -Wmissing-include-dirs -Wswitch-default -Wlarger-than-4096 -Wundef )

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

    set_source_files_properties( ${CPP_SOURCES} ${PLATFORM_CPP_SOURCES} PROPERTIES
        COMPILE_FLAGS -march=native )
    set_source_files_properties( ${PLATFORM_MM_SOURCES} PROPERTIES
        COMPILE_FLAGS -march=core2 LANGUAGE C )
    set( ${NAME} ${CPP_SOURCES} ${PLATFORM_CPP_SOURCES} ${PLATFORM_MM_SOURCES} )
endmacro()

function( ooe_properties NAME )
    set_target_properties( ${NAME} PROPERTIES LINK_INTERFACE_LIBRARIES "" LINKER_LANGUAGE C )
endfunction()

function( ooe_executable NAME )
    ooe_glob( SOURCES ${ARGN} )
    add_executable( ${NAME} ${SOURCES} )
    set_target_properties( ${NAME} PROPERTIES BUILD_WITH_INSTALL_RPATH ON
        INSTALL_RPATH \${ORIGIN}/../lib )
    ooe_properties( ${NAME} )
endfunction()

function( ooe_library NAME )
    ooe_glob( SOURCES ${ARGN} )
    add_library( ${NAME} ${SOURCES} )
    set_target_properties( ${NAME} PROPERTIES BUILD_WITH_INSTALL_RPATH ON INSTALL_NAME_DIR @rpath )
    ooe_properties( ${NAME} )
endfunction()

function( ooe_module NAME )
    ooe_glob( SOURCES ${ARGN} )
    add_library( ${NAME} MODULE ${SOURCES} )
    ooe_properties( ${NAME} )
endfunction()
