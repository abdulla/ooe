/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_MEMORY_HPP
#define OOE_FOUNDATION_IO_MEMORY_HPP

#include "foundation/io/descriptor.hpp"

namespace ooe
{
	struct memory_id
	{
		void* const area;
		const up_t size;

		memory_id( u8, up_t, s32 );
		~memory_id( void ) OOE_VISIBLE;
	};

	class OOE_VISIBLE memory
		: public descriptor
	{
	public:
		enum
		{
			none		= 0,
			read		= 1 << 0,
			write		= 1 << 1,
			execute		= 1 << 2,

			read_write	= read | write
		};

		enum advise_type
		{
			normal,
			random,
			sequential,
			will_need,
			dont_need
		};

		memory( const descriptor&, u8 = read, up_t = 0 );

		void advise( advise_type ) const;
		void sync( void ) const;

		void protect( u8 );

		up_t size( void ) const;
		void* get( void ) const;

		template< typename to >
			to* as( void ) const
		{
			return reinterpret_cast< to* >( get() );
		}

	private:
		shared_ptr< memory_id > internal;
	};
}

#endif	// OOE_FOUNDATION_IO_MEMORY_HPP
