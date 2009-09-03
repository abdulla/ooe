class ooe:
	compiler = 'g++-4.2'
	include_path = [ '/sw/include' ]
	library_path = [ '/sw/lib' ]
	link_flags = [ '-Wl,-undefined,dynamic_lookup' ]

	class appkit:
		framework = [ 'AppKit' ]

	class dl:
		library = []

	class exr:
		include_path = [ '/sw/include/OpenEXR' ]

	class freetype:
		include_path = [ '/sw/lib/freetype219/include', '/sw/lib/freetype219/include/freetype2' ]
		library_path = [ '/sw/lib/freetype219/lib' ]

	class gl:
		library = []

	class lua:
		include_path = []
		library = [ 'lua.5.1' ]

	class pthread:
		library = []

	class qtkit:
		framework = [ 'QTKit' ]

	class rt:
		library = []

	class x11:
		library = []
