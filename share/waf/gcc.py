class compiler:
    defines = []
    flags = [ '-std=c++98', '-pedantic-errors', '-pipe', '-fstrict-aliasing',
        '-funit-at-a-time', '-fuse-cxa-atexit',

        '-Wall', '-Wcast-align', '-Werror', '-Wextra', '-Wfatal-errors', '-Wfloat-equal',
        '-Wformat=2', '-Wmissing-include-dirs', '-Wno-long-long', '-Wnon-virtual-dtor',
        '-Woverloaded-virtual', '-Wpointer-arith', '-Wreorder', '-Wshadow', '-Wstrict-aliasing',
        '-Wswitch-default', '-Wundef', '-Wwrite-strings' ]

    class debug:
        defines = [ '_FORTIFY_SOURCE=2' ]
        flags = [ '-O0', '-g2', '-fno-inline', '-fstack-protector-all' ]

    class release:
        defines = []
        flags = [ '-O3', '-g0', '-fomit-frame-pointer', '-ffast-math', '-ftracer', '-fweb',
            '-fvisibility=hidden', '-fvisibility-inlines-hidden' ]
