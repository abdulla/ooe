/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "external/service/sight.hpp"

namespace ooe
{
//--- sight_provider -----------------------------------------------------------
	sight_provider::sight_provider( u16 width, u16 height )
		: sight(), size( width * height * 4 ), frame( new u8[ size ] ), state_( false ),
		sight_( sight::call_type( *this, &sight_provider::call ), width, height )
	{
	}

	bool sight_provider::state( void )
	{
		return state_;
	}

	void sight_provider::done( void )
	{
		state_ = false;
	}

	void sight_provider::call( const u8* data )
	{
		std::copy( data, data + size, frame.get() );
		state_ = true;
	}

	const u8* sight_provider::get( void ) const
	{
		return frame.get();
	}

//--- sight_service ------------------------------------------------------------
	void sight_service( void* pointer )
	{
		sight_provider& provider = *static_cast< sight_provider* >( pointer );

		if ( !provider.state() )
			return;

		provider.done();
		provider.sight( provider.get() );
	}

//--- sight_register -----------------------------------------------------------
	void sight_register( service& service, sight_provider& provider )
	{
		service.insert( sight_service, &provider );
		service.insert( "sight", provider.sight );
	}
}
