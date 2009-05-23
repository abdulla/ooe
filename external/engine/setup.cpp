/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <unistd.h>

#include "foundation/general/video.hpp"
#include "foundation/io/vfs.hpp"
#include "foundation/lua/table.hpp"
#include "foundation/lua/vm.hpp"
#include "foundation/utility/error.hpp"

#include "external/engine/interface.hpp"
#include "external/engine/setup.hpp"

namespace ooe
{
	bool setup_option( path& path, s32 argc, c8** argv )
	{
		path.vfs = path.root + "../resource/vfs.lua";
		path.settings = "settings.lua";
		path.runtime = path.root + "../library/libruntime";
		up_t i;
		up_t j;

		for ( s32 option; ( option = getopt( argc, argv, "v:s:r:h" ) ) != -1; )
		{
			switch ( option )
			{
			case 'v':
				path.vfs = optarg;
				break;

			case 's':
				path.settings = optarg;
				break;

			case 'r':
				path.runtime = optarg;
				i = path.runtime.rfind( '/' );
				j = path.runtime.rfind( '.' );
				path.runtime = path.runtime.substr( 0, i < j ? j : std::string::npos );
				break;

			default:
				std::cout <<
					"-v <script>     Load virtual file system from script\n"
					"-s <script>     Load settings from script\n"
					"-r <library>    Load runtime from library\n"
					"-h              This help message\n";

				return false;
			}
		}

		return true;
	}

	void setup_vfs( vfs& vfs, const path& path )
	{
		lua::vm vm;
		lua::stack stack = vm.stack();

		lua::table global( stack, lua::global );
		global[ "root" ] = path.root;
		vm.load( "setup_vfs", descriptor( path.vfs ) );

		typedef lua::table::iterator iterator;
		lua::table table = global[ "table" ];

		for ( iterator i = table.begin(), end = table.end(); i != end; ++i )
		{
			const iterator::value_type& value = *i;
			vfs.insert( value.first, value.second );
		}

		if ( vfs.empty() )
			throw error::runtime( "setup: " ) << "VFS descriptor is invalid";
	}

	void setup_settings( settings& set, const descriptor& desc )
	{
		lua::vm vm;
		vm.load( "setup_settings", desc );
		lua::table global( vm.stack(), lua::global );

		lua::table service = global[ "service" ];
		set.service.path = static_cast< const std::string& >( service[ "path" ] );
		set.service.input = service[ "input" ];
		set.service.sight = service[ "sight" ];

		lua::table view = global[ "view" ];
		set.view.width = view[ "width" ];
		set.view.height = view[ "height" ];
		set.view.full = view[ "full" ];

		lua::table video = global[ "video" ];
		set.video.path = static_cast< const std::string& >( video[ "path" ] );
		set.video.sync = video[ "sync" ];

		lua::table sight = global[ "sight" ];
		set.sight.width = sight[ "width" ];
		set.sight.height = sight[ "height" ];
	}

	void setup_video( video& video, u16 width, u16 height, bool sync )
	{
		video.sync( sync );
		video.clear();
		video.viewport( width, height );
		video.depth( video::less_equal );

		video.enable( video::blend_mode, false );
		video.enable( video::face_cull, false );
		video.enable( video::depth_test );
	}
}
