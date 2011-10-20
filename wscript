#!/usr/bin/env python
# encoding: utf-8

import os
import preproc
import sys
import waflib

preproc.strict_quotes = 1
top = '.'
out = '/' + os.readlink( '/tmp' ) + '/ooe'

def options( context ):
    context.load( 'compiler_c compiler_cxx' )
    group = context.add_option_group( 'OOE Build Options' )

    group.add_option(
        '-d',
        '--debug',
        action = 'store_true',
        default = True,
        dest = 'debug',
        help = 'Configures a debug build [default: %default]' )

    group.add_option(
        '-r',
        '--release',
        action = 'store_false',
        dest = 'debug',
        help = 'Configures a release build' )

def configure( context ):
    context.load( 'compiler_c compiler_cxx' )
    context.env.CXXFLAGS = [ '-std=c++98', '-pedantic-errors', '-pipe', '-fstrict-aliasing',
        '-funit-at-a-time', '-fuse-cxa-atexit',

        '-Wall', '-Wcast-align', '-Werror', '-Wextra', '-Wfatal-errors', '-Wfloat-equal',
        '-Wformat=2', '-Wmissing-include-dirs', '-Wno-long-long', '-Wnon-virtual-dtor',
        '-Woverloaded-virtual', '-Wpointer-arith', '-Wreorder', '-Wshadow', '-Wstrict-aliasing',
        '-Wswitch-default', '-Wundef', '-Wwrite-strings' ]
    context.env.DEFINES = []
    context.env.INCLUDES = [ '#' ]

    if context.options.debug:
        context.env.DEFINES.append( '_FORTIFY_SOURCE=2' )
        context.env.CXXFLAGS.extend( [ '-O0', '-g2', '-fno-inline', '-fstack-protector-all' ] )
    else:
        context.env.CXXFLAGS.extend( [ '-O3', '-g0', '-fomit-frame-pointer', '-ffast-math -ftracer'
            '-fweb', '-fvisibility=hidden', '-fvisibility-inlines-hidden' ] )

    platform( context )

def build( context ):
    context.recurse( 'foundation' )
    # context.recurse( 'component external foundation test' )

def platform( context ):
    if sys.platform.startswith( 'darwin' ):
        context.env.DEFINES.append( 'OOE_PLATFORM=darwin' )
        context.env.INCLUDES.append( '/sw/include' )
        context.env.LIBPATH = '/sw/lib'
        context.env.RPATH = '@executable_path/../lib'
    elif sys.platform.startswith( 'linux' ):
        context.env.DEFINES.append( 'OOE_PLATFORM=linux' )
        context.env.RPATH = '${ORIGIN}/../lib'
    else:
        context.fatal( 'Unknown platform: ' + sys.platform )

@waflib.TaskGen.extension( '.mm' )
def mm_hook( task_gen, node ):
    return task_gen.create_compiled_task( 'c', node )
