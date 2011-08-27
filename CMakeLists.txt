project( OOE )

cmake_minimum_required( VERSION 2.8 )
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/platform/module )
include( platform/build.cmake )

### packages #######################################################################################
find_package( Boost REQUIRED )
find_package( Freetype REQUIRED )
find_package( JPEGTurbo REQUIRED )
find_package( Lua51 REQUIRED )
find_package( OpenEXR REQUIRED )
find_package( OpenGL REQUIRED )
find_package( OpenJPEG REQUIRED )
find_package( PNG REQUIRED )
find_package( Python REQUIRED )
find_package( Squish REQUIRED )
find_package( TBB REQUIRED )
find_package( Threads REQUIRED )
find_package( UTF8CPP REQUIRED )
find_package( V8 REQUIRED )
find_package( ZLIB REQUIRED )

include_directories( . )
include_directories( ${FREETYPE_INCLUDE_DIRS} )
include_directories( ${JPEGTURBO_INCLUDE_DIRS} )
include_directories( ${LUA_INCLUDE_DIR} )
include_directories( ${OPENGL_INCLUDE_DIR} )
include_directories( ${OPENJPEG_INCLUDE_DIRS} )
include_directories( ${PNG_INCLUDE_DIR} )
include_directories( ${PYTHON_INCLUDE_DIRS} )
include_directories( ${SQUISH_INCLUDE_DIRS} )
include_directories( ${TBB_INCLUDE_DIRS} )
include_directories( ${ZLIB_INCLUDE_DIRS} )
ooe_directories( ${Boost_INCLUDE_DIRS} )
ooe_directories( ${OPENEXR_INCLUDE_DIRS} )
ooe_directories( ${UTF8CPP_INCLUDE_DIRS} )
ooe_directories( ${V8_INCLUDE_DIRS} )

### data ###########################################################################################
ooe_files( ${PROJECT_SOURCE_DIR}/share ${PROJECT_BINARY_DIR}/share )
file( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/cache )
file( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/log )

### foundation: library ############################################################################
ooe_library( executable foundation/executable )
target_link_libraries( executable ${EXECUTABLE_LIBRARIES} )

ooe_library( image foundation/image )
target_link_libraries( image io ${JPEGTURBO_LIBRARIES} ${OPENEXR_LIBRARIES} ${OPENJPEG_LIBRARIES}
    ${PNG_LIBRARIES} ${SQUISH_LIBRARIES} )

ooe_library( io foundation/io )
target_link_libraries( io executable ${ZLIB_LIBRARIES} )

ooe_library( ipc foundation/ipc foundation/ipc/memory foundation/ipc/socket )
target_link_libraries( ipc io parallel )

ooe_library( math foundation/math )

ooe_module( opengl foundation/opengl )
target_link_libraries( opengl visual ${OPENGL_gl_LIBRARY} )

ooe_library( parallel foundation/parallel )
target_link_libraries( parallel executable ${CMAKE_THREAD_LIBS_INIT} ${TBB_LIBRARIES} )

ooe_library( visual foundation/visual )
target_link_libraries( visual image parallel ${FREETYPE_LIBRARIES} ${VISUAL_LIBRARIES} )

### component: library #############################################################################
ooe_library( javascript component/javascript )
target_link_libraries( javascript io registry ${V8_LIBRARIES} )

ooe_library( lua component/lua )
target_link_libraries( lua io registry ${LUA_LIBRARIES} )

ooe_library( python component/python )
target_link_libraries( python io registry ${PYTHON_LIBRARIES} )

ooe_library( registry component/registry )
target_link_libraries( registry ipc )

ooe_static( ui component/ui )
target_link_libraries( ui visual )

### component: executable ##########################################################################
ooe_executable( doc component/registry/doc )
target_link_libraries( doc executable registry )

ooe_executable( javascript_host component/javascript/host )
target_link_libraries( javascript_host executable javascript registry )

ooe_executable( lua_host component/lua/host )
target_link_libraries( lua_host executable lua registry )

ooe_executable( python_host component/python/host )
target_link_libraries( python_host executable python registry )

ooe_executable( registry_server component/registry/server )
target_link_libraries( registry_server executable registry )
set_target_properties( registry_server PROPERTIES OUTPUT_NAME registry )

ooe_executable( retile component/ui/retile )
target_link_libraries( retile executable ui )

ooe_executable( surrogate component/registry/surrogate )
target_link_libraries( surrogate executable registry )

ooe_executable( tile component/ui/tile )
target_link_libraries( tile executable ui )

ooe_executable( ui_server component/ui/server )
target_link_libraries( ui_server executable math ui )
set_target_properties( ui_server PROPERTIES OUTPUT_NAME ui )

### test: library ##################################################################################
ooe_library( unit test/unit )
target_link_libraries( unit executable io )

### test: executable ###############################################################################
# mock
ooe_executable( mock test/mock )
target_link_libraries( mock executable )

# component
ooe_executable( registry_test test/component/registry )
target_link_libraries( registry_test javascript lua python registry unit )

ooe_executable( ui_test test/component/ui )
target_link_libraries( ui_test math ui unit )

# foundation
ooe_executable( image_test test/foundation/image )
target_link_libraries( image_test image unit )

ooe_executable( io_test test/foundation/io )
target_link_libraries( io_test parallel unit )

ooe_executable( ipc_test test/foundation/ipc )
target_link_libraries( ipc_test ipc unit )

ooe_executable( math_test test/foundation/math )
target_link_libraries( math_test math unit )

ooe_executable( opengl_test test/foundation/opengl )
target_link_libraries( opengl_test math unit visual )

ooe_executable( parallel_test test/foundation/parallel )
target_link_libraries( parallel_test parallel unit )

ooe_executable( utility_test test/foundation/utility )
target_link_libraries( utility_test unit )

### external: library ##############################################################################
ooe_module( hello external/hello )
target_link_libraries( hello javascript lua python registry )

### external: executable ###########################################################################
ooe_executable( itctopng external/itctopng )
target_link_libraries( itctopng executable io )

ooe_executable( hello_server external/hello/server )
target_link_libraries( hello_server executable ipc )
set_target_properties( hello_server PROPERTIES OUTPUT_NAME hello )

ooe_executable( memoryrpc_client external/memoryrpc/client )
target_link_libraries( memoryrpc_client executable ipc )

ooe_executable( memoryrpc_input external/memoryrpc/input )
target_link_libraries( memoryrpc_input executable ipc )

ooe_executable( memoryrpc_noop external/memoryrpc/noop )
target_link_libraries( memoryrpc_noop executable ipc )

ooe_executable( memoryrpc_output external/memoryrpc/output )
target_link_libraries( memoryrpc_output executable ipc )

ooe_executable( memoryrpc_server external/memoryrpc/server )
target_link_libraries( memoryrpc_server executable ipc )

ooe_executable( monitor external/monitor )
target_link_libraries( monitor executable image visual )

ooe_executable( pool_bench external/pool_bench )
target_link_libraries( pool_bench executable parallel )

ooe_executable( socketrpc_input external/socketrpc/input )
target_link_libraries( socketrpc_input executable ipc )

ooe_executable( socketrpc_noop external/socketrpc/noop )
target_link_libraries( socketrpc_noop executable ipc )

ooe_executable( socketrpc_output external/socketrpc/output )
target_link_libraries( socketrpc_output executable ipc )

ooe_executable( socketrpc_server external/socketrpc/server )
target_link_libraries( socketrpc_server executable ipc )

ooe_executable( transport_notify external/transport/notify )
target_link_libraries( transport_notify executable ipc )

ooe_executable( transport_wait external/transport/wait )
target_link_libraries( transport_wait executable ipc )
