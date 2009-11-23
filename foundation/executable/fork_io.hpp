/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_FORK_IO_HPP
#define OOE_FOUNDATION_EXECUTABLE_FORK_IO_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
//--- fork_id ------------------------------------------------------------------
	struct fork_id
	{
		s32 read;
		s32 write;
		pid_t pid;

		fork_id( void );
		~fork_id( void );
	};

//--- fork_io ------------------------------------------------------------------
	class OOE_VISIBLE fork_io
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

		static void execute( const std::string&, ... ) OOE_NORETURN OOE_SENTINEL;
		static void exit( bool ) OOE_NORETURN;

	private:
		shared_ptr< const fork_id > id;
	};

	std::string read( fork_io& ) OOE_VISIBLE;
	fork_io& operator <<( fork_io&, const c8* ) OOE_VISIBLE;
	fork_io& operator <<( fork_io&, const std::string& ) OOE_VISIBLE;
}

#endif	// OOE_FOUNDATION_EXECUTABLE_FORK_IO_HPP
