import sys

### flags ######################################################################
flags_debug = Split( '-O0 -g2 -fno-inline -fstack-protector-all -D_FORTIFY_SOURCE=2' )

flags_release = Split( '-O3 -g0 -fomit-frame-pointer -ffast-math -ftracer -fweb '
	'-fvisibility=hidden -fvisibility-inlines-hidden' )
	# visibility is set in release build, otherwise backtrace() won't work in debug build

flags_cxx = Split( '-pipe -ansi -pedantic-errors -fno-enforce-eh-specs -fuse-cxa-atexit '
	' -funit-at-a-time -fstrict-aliasing -mfpmath=sse -msse3' )
	# -frepo

flags_cxx += Split( '-Wall -Wextra -Werror -Wshadow -Wfloat-equal -Wnon-virtual-dtor -Wcast-align '
	'-Woverloaded-virtual -Wreorder -Wpointer-arith -Wwrite-strings -Wno-long-long -Wformat=2 '
	'-Wstrict-aliasing -Wmissing-include-dirs -Wswitch-default -Wlarger-than-8192 -Wundef' )
	# -Wold-style-cast

### setup ######################################################################
root = Environment( CPPPATH = '#', LIBPATH = '#library', CXXFLAGS = flags_cxx )
#root.SetOption( 'silent', True )
#root.SetOption( 'num_jobs', 4 )

colour = { 'cyan': '\033[46m', 'green': '\033[42m', 'yellow': '\033[43m', 'none': '\033[0m' }

if not sys.stdout.isatty():
	for key in colour.iterkeys():
		colour[ key ] = ''

root[ 'CXXCOMSTR' ] = '\t%s[CC]%s $SOURCE' % ( colour[ 'green' ], colour[ 'none' ] )
root[ 'SHCXXCOMSTR' ] = '\t%s[CC]%s $SOURCE' % ( colour[ 'green' ], colour[ 'none' ] )
root[ 'LINKCOMSTR' ] = '\t%s[LD]%s $TARGET' % ( colour[ 'cyan' ], colour[ 'none' ] )
root[ 'SHLINKCOMSTR' ] = '\t%s[LD]%s $TARGET' % ( colour[ 'yellow' ], colour[ 'none' ] )

### arguments ##################################################################
vars = Variables()
vars.Add( EnumVariable( 'build', 'build type', 'debug', ( 'debug', 'release' ) ) )
Help( vars.GenerateHelpText( root ) )
root.Replace( variables = vars )

build = ARGUMENTS.get( 'build', 'debug' )
print 'Build:', build

if build == 'release':
	root.Append( CXXFLAGS = flags_release )
else:
	root.Append( CXXFLAGS = flags_debug )

### platform ###################################################################
name = root[ 'PLATFORM' ]

if name == 'posix':
	from platform.posix import *
	root.Replace( CXX = ooe.compiler )
	if build == 'release': root.Append( LINKFLAGS = Split( '-Wl,--strip-all -Wl,--gc-sections' ) )
elif name == 'darwin':
	root.Replace( CXX = ooe.compiler )
	from platform.darwin import *
else:
	print 'Platform:', name, '->', 'unknown'
	Exit( 1 )

platform = '#/platform/' + name
print 'Platform:', name, '->', platform

root.Append( CPPPATH = [ platform ] + ooe.include_path )
root.Append( LIBPATH = ooe.library_path )
root.Append( SHLINKFLAGS = ooe.link_flags )

### export #####################################################################
Export( Split( 'name ooe platform root' ) )
SConscript( dirs = Split( 'component external foundation test' ) )
