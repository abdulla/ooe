/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SHARED_MEMORY_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SHARED_MEMORY_HPP

#include "foundation/io/memory.hpp"

namespace ooe
{
	namespace ipc
	{
		struct shared_memory_id;
		class shared_memory_base;
		struct shared_memory;
	}

//--- ipc::shared_memory_id ----------------------------------------------------
	struct ipc::shared_memory_id
	{
		std::string name;

		shared_memory_id( const std::string& );
		~shared_memory_id( void ) OOE_VISIBLE;
	};

//--- ipc::shared_memory_base --------------------------------------------------
	class ipc::shared_memory_base
	{
	public:
		shared_memory_base( const std::string&, bool );

	protected:
		scoped_ptr< shared_memory_id > id;
	};

//--- ipc::shared_memory -------------------------------------------------------
	struct OOE_VISIBLE ipc::shared_memory
		: private shared_memory_base, private memory
	{
		enum type
		{
			open,
			create
		};

		shared_memory( const std::string&, type = open, up_t = 0 );
		std::string name( void ) const;
		void unlink( void );

		using memory::as;
		using memory::get;
		using memory::size;
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_SHARED_MEMORY_HPP
