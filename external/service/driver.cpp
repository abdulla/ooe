/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/macro.hpp"

#include "external/engine/dynamic.hpp"
#include "external/engine/interface.hpp"
#include "external/service/input.hpp"
#include "external/service/sight.hpp"

namespace
{
	using namespace ooe;

	input_provider* set_input( const settings& set, register_type& reg )
	{
		if ( !set.service.input )
			return 0;

		scoped_ptr< input_provider > input( new input_provider( reg.queue ) );
		input_register( reg.service, *input );
		return input.release();
	}

	sight_provider* set_sight( const settings& set, register_type& reg )
	{
		if ( !set.service.sight )
			return 0;

		scoped_ptr< sight_provider >
			sight( new sight_provider( set.sight.width, set.sight.height ) );

		sight_register( reg.service, *sight );
		return sight.release();
	}

	struct provider
	{
		scoped_ptr< input_provider > input;
		scoped_ptr< sight_provider > sight;

		provider( const settings& set, register_type& reg )
			: input( set_input( set, reg ) ), sight( set_sight( set, reg ) )
		{
		}
	};

	void close( void* pointer )
	{
		delete static_cast< provider* >( pointer );
	}
}

extern "C" dynamic::close_type OOE_VISIBLE ooe_open( const settings& set, register_type& reg )
{
	return dynamic::close_type( new provider( set, reg ), close );
}
