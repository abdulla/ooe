### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###

import sys

module = sys.modules.get('platform')
if module: reload(module)

from platform.build import *

Import( 'variables' )

### configure ######################################################################################
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

def PythonConfigure( platform, setup ):
    if not setup.CheckLibWithHeader( 'python3.1', 'python3.1/Python.h', 'c' ): Exit( 1 )

def UtilityConfigure( platform, setup ):
    if not setup.CheckCHeader( 'execinfo.h' ): Exit( 1 )
    if not setup.CheckCXXHeader( 'boost/call_traits.hpp' ): Exit( 1 )
    if not setup.CheckCXXHeader( 'boost/noncopyable.hpp' ): Exit( 1 )
    if not setup.CheckCXXHeader( 'boost/preprocessor.hpp' ): Exit( 1 )
    if not setup.CheckCXXHeader( 'boost/static_assert.hpp' ): Exit( 1 )
    if not setup.CheckCXXHeader( 'boost/type_traits.hpp' ): Exit( 1 )
    if not setup.CheckCXXHeader( 'boost/mpl/vector.hpp' ): Exit( 1 )
    if not setup.CheckCXXHeader( 'boost/utility/enable_if.hpp' ): Exit( 1 )

def VisualConfigure( platform, setup ):
    if not setup.CheckLibWithHeader( 'freetype', 'ft2build.h', 'c' ): Exit( 1 )

    if platform == 'posix':
        if not setup.CheckLibWithHeader( 'v4lconvert', 'libv4lconvert.h', 'c' ): Exit( 1 )
        if not setup.CheckLib( 'Xrandr' ): Exit( 1 )

### build ##########################################################################################
build = Build( variables )
exec 'from platform.' + build.platform + ' import *'

#--- foundation ------------------------------------------------------------------------------------
build.Configurable( configure = UtilityConfigure )

build.Linkable( 'executable', 'foundation/executable', frameworks = ooe.appkit.framework,
    configure = ExecutableConfigure )
build.Linkable( 'image', 'foundation/image', 'io', include_path = ooe.exr.include_path,
    configure = ImageConfigure )
build.Linkable( 'io', 'foundation/io', configure = IOConfigure )
build.Linkable( 'ipc', 'foundation/ipc foundation/ipc/memory foundation/ipc/socket', 'io parallel' )
build.Linkable( 'maths', 'foundation/maths' )
build.Linkable( 'opengl', 'foundation/opengl', 'io', configure = OpenGLConfigure )
build.Linkable( 'parallel', 'foundation/parallel', configure = ParallelConfigure )
build.Linkable( 'visual', 'foundation/visual', 'image parallel', ooe.qtkit.framework,
    ooe.freetype.include_path, configure = VisualConfigure )

#--- component -------------------------------------------------------------------------------------
build.Executable( 'doc', 'component/registry/doc', 'executable registry' )
build.Executable( 'javascript_host', 'component/javascript/host', 'executable javascript registry' )
build.Executable( 'lua_host', 'component/lua/host', 'executable lua registry' )
build.Executable( 'python_host', 'component/python/host', 'executable python registry' )
build.Executable( 'registry', 'component/registry/server', 'executable registry' )
build.Executable( 'surrogate', 'component/registry/surrogate', 'executable registry' )
build.Executable( 'ui', 'component/ui', 'executable maths visual' )

build.Linkable( 'javascript', 'component/javascript', 'io', configure = JavaScriptConfigure )
build.Linkable( 'lua', 'component/lua', 'io', include_path = ooe.lua.include_path,
    configure = LuaConfigure )
build.Linkable( 'python', 'component/python', 'io', library_path = ooe.python.library_path,
    configure = PythonConfigure )
build.Linkable( 'registry', 'component/registry', 'ipc' )

#--- test ------------------------------------------------------------------------------------------
build.Executable( 'registry_test', 'test/component/registry',
    'javascript lua python registry unit' )
build.Executable( 'image_test', 'test/foundation/image', 'image unit' )
build.Executable( 'io_test', 'test/foundation/io', 'parallel unit' )
build.Executable( 'ipc_test', 'test/foundation/ipc', 'ipc unit' )
build.Executable( 'maths_test', 'test/foundation/maths', 'maths unit' )
build.Executable( 'opengl_test', 'test/foundation/opengl', 'maths unit visual' )
build.Executable( 'parallel_test', 'test/foundation/parallel', 'parallel unit' )
build.Executable( 'utility_test', 'test/foundation/utility', 'unit' )

build.Linkable( 'mock', 'test/mock' )
build.Linkable( 'unit', 'test/unit', 'io executable' )

#--- external --------------------------------------------------------------------------------------
build.Executable( 'hello', 'external/hello/server', 'executable ipc' )
build.Executable( 'memoryrpc_input', 'external/memoryrpc/input', 'executable ipc' )
build.Executable( 'memoryrpc_noop', 'external/memoryrpc/noop', 'executable ipc' )
build.Executable( 'memoryrpc_output', 'external/memoryrpc/output', 'executable ipc' )
build.Executable( 'memoryrpc_server', 'external/memoryrpc/server', 'executable ipc' )
build.Executable( 'monitor', 'external/monitor', 'executable image visual' )
build.Executable( 'socketrpc_input', 'external/socketrpc/input', 'executable ipc' )
build.Executable( 'socketrpc_noop', 'external/socketrpc/noop', 'executable ipc' )
build.Executable( 'socketrpc_output', 'external/socketrpc/output', 'executable ipc' )
build.Executable( 'socketrpc_server', 'external/socketrpc/server', 'executable ipc' )

build.Linkable( 'hello', 'external/hello', 'javascript lua python registry' )
