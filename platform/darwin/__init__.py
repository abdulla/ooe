### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###

class ooe:
	compiler = 'platform/darwin/compiler'
	include_path = '/sw/include'
	library_path = '/sw/lib'
	cxx_flags = '-march=core2'
	# disabled link flags: -Wl,-undefined,dynamic_lookup
	link_flags = ''
	release_flags = ''

	class appkit:
		framework = 'AppKit'

	class exr:
		include_path = '/sw/include/OpenEXR'

	class freetype:
		include_path = '/sw/lib/freetype219/include /sw/lib/freetype219/include/freetype2'
		library_path = '/sw/lib/freetype219/lib'

	class lua:
		include_path = ''
		library = 'lua.5.1'

	class qtkit:
		framework = 'QTKit'
