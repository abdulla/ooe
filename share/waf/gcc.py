## Copyright (C) 2012 Abdulla Kamar. All rights reserved. ##

import waflib

class compiler:
    defines = []
    flags = [ '-std=c++98', '-pedantic-errors', '-pipe', '-fstrict-aliasing', '-funit-at-a-time',
        '-fuse-cxa-atexit', '-fvisibility=hidden', '-fvisibility-inlines-hidden',

        '-Wall', '-Wcast-align', '-Werror', '-Wextra', '-Wfatal-errors', '-Wfloat-equal',
        '-Wformat=2', '-Wmissing-include-dirs', '-Wno-long-long', '-Wnon-virtual-dtor',
        '-Woverloaded-virtual', '-Wpointer-arith', '-Wreorder', '-Wshadow', '-Wswitch-default',
        '-Wundef', '-Wwrite-strings' ]

    class debug:
        defines = [ '_FORTIFY_SOURCE=2' ]
        flags = [ '-O0', '-g2', '-fno-common', '-fno-inline', '-fstack-protector-all' ]

    class release:
        defines = []
        flags = [ '-O2', '-g0', '-fomit-frame-pointer' ]

@waflib.TaskGen.feature( 'c' )
@waflib.TaskGen.feature( 'cxx' )
@waflib.TaskGen.before( 'process_source' )
def system_includes_hook( task_gen ):
    for i in task_gen.to_list( getattr( task_gen, 'system_includes', [] ) ):
        flags = [ '-isystem', i ]
        task_gen.env.CFLAGS.extend( flags )
        task_gen.env.CXXFLAGS.extend( flags )
