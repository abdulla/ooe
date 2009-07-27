/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <csignal>

#include "foundation/executable/environment.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/general/event_queue.hpp"
#include "foundation/general/video.hpp"
#include "foundation/general/view.hpp"
#include "foundation/io/vfs.hpp"
#include "foundation/parallel/scheduler.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

#include "external/engine/dynamic.hpp"
#include "external/engine/interface.hpp"
#include "external/engine/setup.hpp"
#include "external/service/service.hpp"

namespace
{
	using namespace ooe;

	typedef ooe::signal< void ( void ) > emergency_signal;
	typedef slot< void ( void ) > emergency_slot;

	emergency_signal emergency;

	template< typename type >
		void setup_emergency( type& object, emergency_slot& slot )
	{
		slot = emergency_slot::function_type( object, destruct< type > );
		emergency.push_back( slot );
	}

	void emergency_handler( void )
	{
		emergency();
	}

	void timer_unload( timer& timer )
	{
		std::cerr << "Unloaded in " << timer.elapsed() << " seconds\n";
	}

	void dynamic_unload( dynamic& dynamic )
	{
		tuple< up_t, up_t > size = dynamic.size();
		dynamic.clear();

		std::cerr <<
			"Destroyed dynamic loader\n"
			"\tlibraries: " << size._0 << "\n"
			"\tdestructors: " << size._1 << "\n";
	}

	std::string name_only( const std::string& name )
	{
		up_t i = name.rfind( '/' );
		i = i == std::string::npos ? 0 : i + 1;
		up_t j = name.rfind( '.' );
		j = j == std::string::npos || j < i ? std::string::npos : j - i;
		return name.substr( i, j );
	}

	void inform( const path& path, const settings& set, const scheduler& scheduler, timer& timer )
	{
		std::cerr <<
			"Created virtual file system from \"" << name_only( path.vfs ) << ".lua\"\n"
			"Loaded settings from \"" << name_only( path.settings ) << ".lua\"\n"
			"Created task scheduler\n"
			"\tsize: " << scheduler.size() << "\n"
			"Created view\n"
			"\twidth: " << set.view.width << "\n"
			"\theight: " << set.view.height << "\n";

		if ( set.service.sight )
		{
			std::cerr <<
				"Created sight device\n"
				"\twidth: " << set.sight.width << "\n"
				"\theight: " << set.sight.height << "\n";
		}

		std::cerr <<
			"Created video system from \"" << name_only( set.video.path ) << "\"\n"
			"Created runtime from \"" << name_only( path.runtime ) << "\"\n"
			"Created services from \"" << name_only( set.service.path ) << "\"\n"
			"Loaded in " << timer.elapsed() << " seconds\n";
	}

	bool launch( const std::string& root, const std::string&, s32 argc, c8** argv )
	{
		executable::emergency( emergency_handler );
		path path = { root, std::string(), std::string(), std::string() };

		if ( !setup_option( path, argc, argv ) )
			return false;

		timer timer;
		guard< void ( ooe::timer& ) > guard_timer( timer_unload, timer );

		vfs vfs;
		setup_vfs( vfs, path );

		settings set;
		setup_settings( set, vfs[ path.settings ] );
		scheduler scheduler;

		dynamic dynamic;
		dynamic.load( path.root + set.video.path, library::global_lazy );
		event_queue queue;
		view view( queue, set.view.width, set.view.height, set.view.full );
		emergency_slot view_slot;
		setup_emergency( view, view_slot );

		video& video = dynamic.load< ooe::video >( path.root + set.video.path, "ooe_open", view );
		setup_video( video, set.view.width, set.view.height, set.video.sync );

		second_signal second_signal;
		frame_signal frame_signal;

		service service;
		service.insert( "second", second_signal );
		service.insert( "frame", frame_signal );

		register_type reg = { service, scheduler, queue };
		dynamic.load( path.root + set.service.path, "ooe_open", set, reg );

		core_type core = { vfs, scheduler, video };
		dynamic.load( path.runtime, "ooe_open", core, set, service );

		inform( path, set, scheduler, timer );
		guard< void ( ooe::dynamic& ) > guard_dynamic( dynamic_unload, dynamic );
		std::time_t epoch = timer.get()._0;

		while ( !executable::signal() )
		{
			service();
			video.clear();
			frame_signal( timer.elapsed() );
			video.swap();
			executable::yield();

			std::time_t seconds = timer.get()._0;
			std::time_t elapsed = seconds - epoch;
			epoch = seconds;

			if ( elapsed )
				second_signal( elapsed );
		}

		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
