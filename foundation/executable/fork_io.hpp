/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_FORK_IO_HPP
#define OOE_FOUNDATION_EXECUTABLE_FORK_IO_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/pointer.hpp"

namespace ooe
{
	namespace executable
	{
		struct fork_id;
		class fork_io;
	}

//--- executable::fork_id ------------------------------------------------------
	struct executable::fork_id
	{
		s32 read;
		s32 write;
		pid_t pid;

		fork_id( void );
		~fork_id( void );
	};

//--- executable::fork_io ------------------------------------------------------
	class executable::fork_io
	{
	public:
		enum wait_type
		{
			failure,
			success,
			waiting
		};

		fork_io( void );
		~fork_io( void );

		up_t read( void*, up_t );
		up_t write( const void*, up_t );
		wait_type wait( bool = true ) const;
		void signal( s32 ) const;
		bool is_child( void ) const;

		static void exit( bool );

	private:
		shared_ptr< const fork_id > id;
	};
}

#endif	// OOE_FOUNDATION_EXECUTABLE_FORK_IO_HPP
