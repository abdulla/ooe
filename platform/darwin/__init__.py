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

	class dl:
		library = ''

	class exr:
		include_path = '/sw/include/OpenEXR'

	class freetype:
		include_path = '/sw/lib/freetype219/include /sw/lib/freetype219/include/freetype2'
		library_path = '/sw/lib/freetype219/lib'

	class gl:
		library = ''

	class lua:
		include_path = ''
		library = 'lua.5.1'

	class pthread:
		library = ''

	class qtkit:
		framework = 'QTKit'

	class rt:
		library = ''

	class x11:
		library = ''
