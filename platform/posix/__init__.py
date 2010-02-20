### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###

class ooe:
	compiler = 'g++'
	include_path = ''
	library_path = ''
	cxx_flags = '-march=native'
	link_flags = '-Wl,--allow-shlib-undefined -Wl,--as-needed'
	release_flags = '-Wl,--strip-all -Wl,--gc-sections'

	class appkit:
		framework = ''

	class exr:
		include_path = '/usr/include/OpenEXR'

	class freetype:
		include_path = '/usr/include/freetype2'
		library_path = ''

	class lua:
		include_path = '/usr/include/lua5.1'
		library = 'lua5.1'

	class pthread:
		library = 'pthread'

	class python:
		library_path = ''

	class qtkit:
		framework = ''
