import sys

from SCons.Environment import Environment
from SCons.Script import Glob, Help
from SCons.Script.SConscript import Configure
from SCons.Util import Split

class Build( object ):
	"""
	Build object.
	"""
	DEBUG_DEFINES	= Split( '_FORTIFY_SOURCE=2' )
	DEBUG_FLAGS		= Split( '-O0 -g2 -fno-inline -fstack-protector-all' )

	# visibility is set in release only, otherwise backtrace() won't work in debug
	RELEASE_FLAGS	= Split( '-O3 -g0 -fomit-frame-pointer -ffast-math -ftracer -fweb '
		'-fvisibility=hidden -fvisibility-inlines-hidden' )

	# disabled flags: -frepo
	FUNCTION_FLAGS	= Split( '-pipe -ansi -pedantic-errors -fno-enforce-eh-specs -fuse-cxa-atexit '
		'-funit-at-a-time -fstrict-aliasing -mfpmath=sse' )

	# disabled flags: -Wold-style-cast
	WARNING_FLAGS	= Split( '-Wall -Wextra -Werror -Wshadow -Wfloat-equal -Wnon-virtual-dtor '
		'-Wcast-align -Woverloaded-virtual -Wreorder -Wpointer-arith -Wwrite-strings '
		'-Wno-long-long -Wformat=2 -Wstrict-aliasing -Wmissing-include-dirs -Wswitch-default '
		'-Wlarger-than-8192 -Wundef' )

	COLOURS = {
		'cyan':		'\033[46m',
		'green':	'\033[42m',
		'yellow':	'\033[43m',
		'none':		'\033[0m'
	}

	"""
	Constructor.
	"""
	def __init__( self, variables ):
		self.environment = Environment( CPPPATH = '#', LIBPATH = '#library',
			CXXFLAGS = self.FUNCTION_FLAGS + self.WARNING_FLAGS )
		self.platform = self.environment[ 'PLATFORM' ]

		exec 'from ' + self.platform + ' import *'
		self.environment.Replace( CXX = ooe.compiler )
		self.environment.Append( CXXFLAGS = Split( ooe.cxx_flags ) )
		self.environment.Append( CPPPATH =
			Split( '#/platform/' + self.platform + ' ' + ooe.include_path ) )
		self.environment.Append( LIBPATH = Split( ooe.library_path ) )
		self.environment.Append( LINKFLAGS = Split( ooe.link_flags ) )

		Help( variables.GenerateHelpText( self.environment ) )
		self.environment.Replace( variables = variables )

		if not sys.stdout.isatty():
			self.environment[ 'CXXCOMSTR' ] =\
				'\t%s[CC]%s $SOURCE' % ( self.COLOURS[ 'green' ], self.COLOURS[ 'none' ] )
			self.environment[ 'SHCXXCOMSTR' ] =\
				'\t%s[CC]%s $SOURCE' % ( self.COLOURS[ 'green' ], self.COLOURS[ 'none' ] )
			self.environment[ 'LINKCOMSTR' ] =\
				'\t%s[LD]%s $TARGET' % ( self.COLOURS[ 'cyan' ], self.COLOURS[ 'none' ] )
			self.environment[ 'SHLINKCOMSTR' ] =\
				'\t%s[LD]%s $TARGET' % ( self.COLOURS[ 'yellow' ], self.COLOURS[ 'none' ] )

	"""
	Configure.
	"""
	def Configure( self, is_release ):
		exec 'from ' + self.platform + ' import *'

		if is_release:
			self.environment.Append( CXXFLAGS = self.RELEASE_FLAGS )
			self.environment.Append( LINKFLAGS = Split( ooe.release_flags ) )
		else:
			self.environment.Append( CXXFLAGS = self.DEBUG_FLAGS )
			self.environment.Append( CPPDEFINES = self.DEBUG_DEFINES )

	"""
	Configurable.
	"""
	def Configurable( self, include_path = None, library_path = None, configure = None ):
		build = self.environment.Clone()

		if include_path:
			build.Append( CPPPATH = Split( include_path ) )

		if library_path:
			build.Append( LIBPATH = Split( library_path ) )

		if build.GetOption( 'clean' ) or build.GetOption( 'help' ):
			return build

		setup = Configure( build )

		if configure:
			configure( self.platform, setup )

		return setup.Finish()

	"""
	Linkable.
	"""
	def Linkable( self, name, directories, libraries = None, frameworks = None,
			include_path = None, library_path = None, configure = None ):
		build = self.Configurable( include_path, library_path, configure )
		files, effects = self.FindFiles( directories )

		if self.platform == 'darwin':
			build.Append( LINKFLAGS = '-Wl,-install_name,@rpath/lib' + name + '.dylib' )

			if frameworks:
				build.Append( FRAMEWORKS = Split( frameworks ) )

		if libraries:
			build.Append( LIBS = Split( libraries ) )

		library = build.SharedLibrary( '#library/lib' + name, files )
		build.SideEffect( effects, library )

	"""
	Executable.
	"""
	def Executable( self, name, directories, libraries = None, frameworks = None,
			include_path = None, library_path = None, configure = None ):
		build = self.Configurable( include_path, library_path, configure )
		files, effects = self.FindFiles( directories )

		if self.platform == 'posix':
			build.Append( RPATH = "'$$$${ORIGIN}/../library'" )
		elif self.platform == 'darwin':
			build.Append( LINKFLAGS = '-Wl,-rpath,@executable_path/../library' )

			if frameworks:
				build.Append( FRAMEWORKS = Split( frameworks ) )

		if libraries:
			build.Append( LIBS = Split( libraries ) )

		program = build.Program( '#binary/' + name, files )
		build.SideEffect( effects, program )
		build.Clean( program, '#binary/' + name + '.log' )

	"""
	Find files.
	"""
	def FindFiles( self, directories ):
		path = 'platform/' + self.platform
		files = []
		effects = []

		for directory in Split( directories ):
			files += Glob( directory + '/*.cpp' )
			files += Glob( path + '/' + directory + '/*.cpp' )
			files += Glob( path + '/' + directory + '/*.mm' )

			effects += Glob( directory + '/*.rpo' )
			effects += Glob( path + '/' + directory + '/*.rpo' )

		return files, effects
