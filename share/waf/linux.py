class platform:
    name = 'linux'

    defines = [ 'OOE_PLATFORM=linux' ]
    includes = []
    libpath = []
    rpath = [ '${ORIGIN}/../lib' ]

    class executable:
        framework = ''

    class image:
        includes = ''
        libpath = ''

    class opengl:
        framework = ''
        lib = 'GL'

    class python:
        lib = 'python3.2'
        libpath = None

    class visual:
        includes = ''
        framework = ''
