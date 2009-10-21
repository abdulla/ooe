/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_DESCRIPTOR_HPP
#define OOE_FOUNDATION_IO_DESCRIPTOR_HPP

#include "foundation/io/descriptor_forward.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
	struct descriptor_id
	{
		const s32 fd;

		descriptor_id( s32 );
		descriptor_id( const std::string&, s32 );
		~descriptor_id( void ) OOE_VISIBLE;
	};

	class OOE_VISIBLE descriptor
		: public platform::descriptor
	{
	public:
		enum
		{
			read		= 1 << 0,
			write		= 1 << 1,
			append		= 1 << 2,
			truncate	= 1 << 3,
			create		= 1 << 4,

			write_new	= write | truncate | create,
			read_write	= read | write | create
		};

		enum node_type
		{
			unknown,

			directory,
			character,
			block,
			file,
			fifo,
			link,
			socket
		};

		descriptor( s32 );
		descriptor( const std::string&, u8 = read );

		node_type type( void ) const;
		up_t size( void ) const;

		void resize( up_t );
		void control( u32, void* );

		s32 get( void ) const;

	protected:

	private:
		shared_ptr< const descriptor_id > id;
	};
}

#endif	// OOE_FOUNDATION_IO_DESCRIPTOR_HPP
