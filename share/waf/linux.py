## Copyright (C) 2012 Abdulla Kamar. All rights reserved. ##

class platform:
    name = 'linux'

    defines = [ 'OOE_PLATFORM=linux', 'OOE_EXTENSION=".so"' ]
    flags = []
    libpath = []
    rpath = [ '${ORIGIN}/../lib' ]

    @staticmethod
    def shlib( name ):
        return ''

    class executable:
        framework = ''

    class image:
        includes = ''
        libpath = ''
        system_includes = ''

    class opengl:
        framework = ''
        lib = 'GL'

    class python:
        lib = 'python3.2'
        libpath = ''

    class ui:
        system_includes = ''

    class visual:
        includes = ''
        framework = ''
