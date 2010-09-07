### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###

class ooe:
	compiler = 'platform/darwin/compiler'
	include_path = '/sw/include'
	library_path = '/sw/lib'
	cxx_flags = '-march=core2'
	link_flags = '-Wl,-undefined,dynamic_lookup'
	release_flags = ''

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
