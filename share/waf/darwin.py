## Copyright (C) 2012 Abdulla Kamar. All rights reserved. ##

import waflib

class platform:
    name = 'darwin'

    defines = [ 'OOE_PLATFORM=darwin', 'OOE_EXTENSION=".dylib"' ]
    flags = [ '-isystem', '/sw/include' ]
    libpath = [ '/sw/lib' ]
    rpath = [ '@executable_path/../lib' ]

    @staticmethod
    def shlib( name ):
        return '-install_name @rpath/lib%s.dylib' % name

    class executable:
        framework = 'AppKit'

    class image:
        includes = '/sw/lib/libjpeg-turbo/include'
        libpath = '/sw/lib/libjpeg-turbo/lib'

    class opengl:
        framework = 'AppKit OpenGL'
        lib = ''

    class python:
        lib = 'python3.3m'
        libpath = '/sw/lib/python3.3/config-3.3m'

    class ui:
        system_includes = '/sw/include/utf8cpp'

    class visual:
        includes = '/sw/include/freetype2'
        framework = 'AppKit CoreVideo QTKit'

@waflib.TaskGen.extension( '.mm' )
def mm_hook( task_gen, node ):
    return task_gen.create_compiled_task( 'c', node )
