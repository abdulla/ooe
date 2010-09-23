### Copyright (C) 2010 Abdulla Kamar. All rights reserved. ###

class ooe:
	compiler = 'g++'
	cxx_flags = '-march=native'
	include_path = ''

	linker = 'g++'
	link_flags = '-Wl,--allow-shlib-undefined -Wl,--as-needed'
	release_flags = '-Wl,--strip-all -Wl,--gc-sections'
	library_path = ''

	class appkit:
		framework = ''

	class exr:
		include_path = '/usr/include/OpenEXR'

	class freetype:
		include_path = '/usr/include/freetype2'

	class lua:
		include_path = '/usr/include/lua5.1'
		library = 'lua5.1'

	class pthread:
		library = 'pthread'

	class python:
		library_path = ''

	class qtkit:
		framework = ''
