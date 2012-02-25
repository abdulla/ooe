#!/usr/bin/env python
## Copyright (C) 2012 Abdulla Kamar. All rights reserved. ##

import os
import preproc
import sys
import waflib

import share.waf

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
    platform, compiler = choose( context )

    context.load( 'compiler_c compiler_cxx' )
    context.env.CFLAGS = compiler.flags + platform.flags
    context.env.CXXFLAGS = context.env.CFLAGS
    context.env.DEFINES = compiler.defines + platform.defines
    context.env.INCLUDES = [ '#' ]
    context.env.LIBPATH = platform.libpath
    context.env.RPATH = platform.rpath

def build( context ):
    platform, _ = choose( context )

    context.env.platform = platform
    context.add_post_fun( install )
    context.recurse( 'component external foundation test' )

def install( context ):
    if context.cmd != 'install':
        return

    context.path.find_or_declare( 'log' ).mkdir()
    context.symlink_as(
        '${PREFIX}/share', context.path.find_dir( 'share' ).abspath(), postpone = False )
#   context.install_files(
#       '${PREFIX}/bin', context.path.find_resource( 'share/xml/Info.plist' ), postpone = False )

def choose( context ):
    if sys.platform.startswith( 'darwin' ):
        from share.waf.darwin import platform
        from share.waf.gcc import compiler
    elif sys.platform.startswith( 'linux' ):
        from share.waf.linux import platform
        from share.waf.gcc import compiler
    else:
        context.fatal( 'Unknown platform: ' + sys.platform )

    variant = compiler.debug if context.options.debug else compiler.release
    compiler.flags.extend( variant.flags )
    compiler.defines.extend( variant.defines )
    return platform, compiler
