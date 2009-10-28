/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <map>

#include "component/registry/registry.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/server.hpp"

namespace
{
	using namespace ooe;

	//--------------------------------------------------------------------------
	typedef std::multimap< module::name_tuple, module::info_tuple > module_map;
	std::string self;
	module_map modules;

	void insert( const module::info_tuple& info, const module::name_vector& names )
	{
		typedef module::name_vector::const_iterator iterator_type;

		for ( iterator_type i = names.begin(), end = names.end(); i != end; ++i )
			modules.insert( module_map::value_type( *i, info ) );
	}

	registry::info_vector find( const interface::name_vector& names )
	{
		typedef interface::name_vector::const_iterator name_iterator;
		typedef std::map< module::info_tuple, up_t > histogram_map;
		histogram_map histogram;

		for ( name_iterator i = names.begin(), end = names.end(); i != end; ++i )
		{
			module_map::const_iterator j = modules.find( *i );

			if ( j == modules.end() )
				continue;

			histogram_map::iterator k = histogram.find( j->second );

			if ( k == histogram.end() )
				histogram.insert( histogram_map::value_type( k->first, 1 ) );
			else
				++k->second;
		}

		typedef histogram_map::const_iterator histogram_iterator;
		registry::info_vector vector;

		for ( histogram_iterator i = histogram.begin(), end = histogram.end(); i != end; ++i )
		{
			if ( i->second == names.size() )
				vector.push_back( i->first );
		}

		return vector;
	}

	//--- registry -------------------------------------------------------------
	void registry( const std::string& root, const std::string& name )
	{
		self = root + name;

		ipc::memory::switchboard switchboard;
		switchboard.insert( insert );
		switchboard.insert( find );

		ipc::memory::server server( "/ooe.registry", switchboard );

		while ( !executable::signal() )
			server.decode();
	}

	//--- surrogate ------------------------------------------------------------
	void surrogate( const std::string& path, const std::string& /*module_name*/ )
	{
		ipc::memory::switchboard switchboard;
		ipc::memory::server server( path, switchboard );

		{
			std::string gate = path + ".g";
			ipc::semaphore semaphore( gate.c_str() );
			semaphore.up();
		}

		while ( !executable::signal() && server.decode() ) {}
	}

	//--- launch ---------------------------------------------------------------
	bool launch( const std::string& root, const std::string& name, s32 /*argc*/, c8** /*argv*/ )
	{
		registry( root, name );
		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
