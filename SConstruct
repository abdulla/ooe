### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###

from platform.build import *

### variables ##################################################################
variables = Variables()
variables.Add( EnumVariable( 'mode', 'build mode', 'debug', ( 'debug', 'release' ) ) )

### configure ##################################################################
def ImageConfigure( platform, setup ):
	if not setup.CheckLibWithHeader( 'jpeg', [ 'stdio.h', 'jpeglib.h' ], 'c' ): Exit( 1 )
	if not setup.CheckLibWithHeader( 'openjpeg', 'openjpeg.h', 'c' ): Exit( 1 )
	if not setup.CheckLibWithHeader( 'png', 'png.h', 'c' ): Exit( 1 )
	if not setup.CheckLibWithHeader( 'squish', 'squish.h', 'c++' ): Exit( 1 )
	if not setup.CheckLib( 'Half', language = 'c++' ): Exit( 1 )
	if not setup.CheckLib( 'Iex', language = 'c++' ): Exit( 1 )
	if not setup.CheckLib( 'IlmImf', language = 'c++' ): Exit( 1 )
	if not setup.CheckLib( 'IlmThread', language = 'c++' ): Exit( 1 )

def ExecutableConfigure( platform, setup ):
	if platform == 'posix':
		if not setup.CheckLib( 'dl', language = 'c' ): Exit( 1 )
		if not setup.CheckLib( 'rt', language = 'c' ): Exit( 1 )

def IOConfigure( platform, setup ):
	if not setup.CheckCXXHeader( 'boost/iterator/iterator_facade.hpp' ): Exit( 1 )
	if not setup.CheckLibWithHeader( 'z', 'zlib.h', 'c' ): Exit( 1 )

	if platform == 'posix':
		if not setup.CheckFunc( 'splice', '#include <fcntl.h>' ): Exit( 1 )

def JavaScriptConfigure( platform, setup ):
	if not setup.CheckLibWithHeader( 'v8', 'v8.h', 'c++' ): Exit( 1 )

def GeneralConfigure( platform, setup ):
	if not setup.CheckLibWithHeader( 'freetype', 'ft2build.h', 'c' ): Exit( 1 )

	if platform == 'posix':
		if not setup.CheckLibWithHeader( 'v4lconvert', 'libv4lconvert.h', 'c' ): Exit( 1 )
		if not setup.CheckLib( 'Xrandr' ): Exit( 1 )

def LuaConfigure( platform, setup ):
	if not setup.CheckLibWithHeader( ooe.lua.library, 'lauxlib.h', 'c' ): Exit( 1 )

def OpenGLConfigure( platform, setup ):
	if platform == 'darwin':
		if not setup.CheckCHeader( 'OpenGL/OpenGL.h' ): Exit( 1 )
	elif platform == 'posix':
		if not setup.CheckLibWithHeader( 'GL', 'GL/glx.h', 'c' ): Exit( 1 )

def ParallelConfigure( platform, setup ):
	if platform == 'posix':
		if not setup.CheckLib( 'pthread', language = 'c' ): Exit( 1 )

def UtilityConfigure( platform, setup ):
	if not setup.CheckCXXHeader( 'boost/call_traits.hpp' ): Exit( 1 )
	if not setup.CheckCXXHeader( 'boost/noncopyable.hpp' ): Exit( 1 )
	if not setup.CheckCXXHeader( 'boost/preprocessor.hpp' ): Exit( 1 )
	if not setup.CheckCXXHeader( 'boost/static_assert.hpp' ): Exit( 1 )
	if not setup.CheckCXXHeader( 'boost/type_traits.hpp' ): Exit( 1 )
	if not setup.CheckCXXHeader( 'boost/mpl/vector.hpp' ): Exit( 1 )
	if not setup.CheckCXXHeader( 'boost/utility/enable_if.hpp' ): Exit( 1 )

### build ######################################################################
build = Build( variables )
build.Configure( 'release' == ARGUMENTS.get( 'mode', 'debug' ) )
exec 'from platform.' + build.platform + ' import *'

#--- foundation ----------------------------------------------------------------
build.Configurable( configure = UtilityConfigure )
build.Linkable( 'executable', 'foundation/executable', frameworks = ooe.appkit.framework,
	configure = ExecutableConfigure )
build.Linkable( 'general', 'foundation/general', 'parallel', ooe.qtkit.framework,
	ooe.freetype.include_path, ooe.freetype.library_path, GeneralConfigure )
build.Linkable( 'image', 'foundation/image', 'io', include_path = ooe.exr.include_path,
	configure = ImageConfigure )
build.Linkable( 'io', 'foundation/io', configure = IOConfigure )
build.Linkable( 'ipc', 'foundation/ipc foundation/ipc/memory foundation/ipc/socket', 'io parallel' )
build.Linkable( 'maths', 'foundation/maths' )
build.Linkable( 'opengl', 'foundation/opengl', configure = OpenGLConfigure )
build.Linkable( 'parallel', 'foundation/parallel', configure = ParallelConfigure )

#--- component -----------------------------------------------------------------
build.Executable( 'lua_host', 'component/lua/host', 'executable lua registry' )
build.Executable( 'javascript_host', 'component/javascript/host', 'executable javascript registry' )
build.Executable( 'registry', 'component/registry/server', 'executable registry' )
build.Linkable( 'javascript', 'component/javascript', 'io', configure = JavaScriptConfigure )
build.Linkable( 'lua', 'component/lua', 'io', include_path = ooe.lua.include_path,
	configure = LuaConfigure )
build.Linkable( 'registry', 'component/registry', 'ipc' )

#--- test ----------------------------------------------------------------------
build.Executable( 'registry_test', 'test/component/registry', 'javascript lua registry unit' )
build.Executable( 'image_test', 'test/foundation/image', 'image unit' )
build.Executable( 'io_test', 'test/foundation/io', 'io parallel unit' )
build.Executable( 'ipc_test', 'test/foundation/ipc', 'ipc unit' )
build.Executable( 'maths_test', 'test/foundation/maths', 'maths unit' )
build.Executable( 'utility_test', 'test/foundation/utility', 'unit' )
build.Linkable( 'unit', 'test/unit', 'executable' )

#--- external ------------------------------------------------------------------
build.Executable( 'chunked', 'external/chunked', 'executable general parallel scene' )
build.Executable( 'hello', 'external/hello/server', 'executable ipc' )
build.Executable( 'memoryrpc_input', 'external/memoryrpc/input', 'executable ipc' )
build.Executable( 'memoryrpc_noop', 'external/memoryrpc/noop', 'executable ipc' )
build.Executable( 'memoryrpc_output', 'external/memoryrpc/output', 'executable ipc' )
build.Executable( 'memoryrpc_server', 'external/memoryrpc/server', 'executable ipc' )
build.Executable( 'monitor', 'external/monitor', 'executable general image' )
build.Executable( 'socketrpc_input', 'external/socketrpc/input', 'executable ipc' )
build.Executable( 'socketrpc_noop', 'external/socketrpc/noop', 'executable ipc' )
build.Executable( 'socketrpc_output', 'external/socketrpc/output', 'executable ipc' )
build.Executable( 'socketrpc_server', 'external/socketrpc/server', 'executable ipc' )
build.Linkable( 'scene', 'external/scene', 'image io maths parallel' )
build.Linkable( 'hello', 'external/hello', 'javascript lua registry' )
