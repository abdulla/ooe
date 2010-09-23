### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###

class ooe:
	compiler = 'platform/darwin/compiler'
	cxx_flags = '-march=core2'
	include_path = '/sw/include'

	linker = 'g++'
	link_flags = '-Wl,-undefined,dynamic_lookup'
	release_flags = ''
	library_path = '/sw/lib'

	class appkit:
		framework = 'AppKit'

	class exr:
		include_path = '/sw/include/OpenEXR'

	class freetype:
		include_path = '/sw/include/freetype2'

	class lua:
		include_path = ''
		library = 'lua.5.1'

	class python:
		library_path = '/sw/lib/python3.1/config'

	class qtkit:
		framework = 'QTKit'
