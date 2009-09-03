class ooe:
	compiler = 'g++'
	include_path = []
	library_path = []
	link_flags = []

	class appkit:
		framework = []

	class dl:
		library = [ 'dl' ]

	class exr:
		include_path = [ '/usr/include/OpenEXR' ]

	class freetype:
		include_path = [ '/usr/include/freetype2' ]
		library_path = []

	class gl:
		library = [ 'GL' ]

	class lua:
		include_path = [ '/usr/include/lua5.1' ]
		library = [ 'lua5.1' ]

	class pthread:
		library = [ 'pthread' ]

	class qtkit:
		framework = []

	class rt:
		library = [ 'rt' ]

	class x11:
		library = [ 'Xrandr', 'Xxf86dga' ]
