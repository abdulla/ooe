class platform:
    name = 'darwin'

    defines = [ 'OOE_PLATFORM=darwin' ]
    includes = [ '/sw/include' ]
    libpath = [ '/sw/lib' ]
    rpath = [ '@executable_path/../lib' ]

    @staticmethod
    def shlib( name ):
        return '-install_name @rpath/lib%s.dylib' % name

    class executable:
        framework = 'AppKit'

    class image:
        includes = '/sw/lib/libjpeg-turbo/include /sw/include/OpenEXR'
        libpath = '/sw/lib/libjpeg-turbo/lib'

    class opengl:
        framework = 'AppKit OpenGL'
        lib = ''

    class python:
        lib = 'python3.2m'
        libpath = '/sw/lib/python3.2/config-3.2m'

    class visual:
        includes = '/sw/include/freetype2'
        framework = 'AppKit CoreVideo QTKit'
