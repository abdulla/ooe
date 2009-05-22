/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_SERVICE_SIGHT_HPP
#define OOE_EXTERNAL_SERVICE_SIGHT_HPP

#include "foundation/general/sight.hpp"
#include "foundation/utility/atom.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/signal.hpp"

#include "external/service/service.hpp"

namespace ooe
{
	typedef signal< void ( const u8* ) > sight_signal;
	typedef slot< void ( const u8* ) > sight_slot;

//--- sight_provider -----------------------------------------------------------
	class sight_provider
	{
	public:
		sight_signal sight;

		sight_provider( u16, u16 );
		bool state( void );
		void done( void );
		const u8* get( void ) const;

	private:
		const up_t size;
		const scoped_array< u8 > frame;

		atom< u32 > state_;
		ooe::sight sight_;

		void call( const u8* );
	};

	void sight_service( void* );
	void sight_register( service&, sight_provider& );
}

#endif	// OOE_EXTERNAL_SERVICE_SIGHT_HPP
