/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SHARED_MEMORY_HPP
#define OOE_FOUNDATION_IPC_SHARED_MEMORY_HPP

#include "foundation/io/memory.hpp"

namespace ooe
{
	namespace ipc
	{
		class shared_memory_base;
		struct shared_memory;

		struct locked_memory;
		class memory_lock;
	}

//--- ipc::shared_memory_base --------------------------------------------------
	class ipc::shared_memory_base
	{
	public:
		std::string name( void ) const OOE_VISIBLE;
		void unlink( void ) OOE_VISIBLE;

	protected:
		shared_memory_base( const std::string&, bool );
		~shared_memory_base( void );

	private:
		std::string name_;
		bool unlinkable;
	};

//--- ipc::shared_memory -------------------------------------------------------
	struct OOE_VISIBLE ipc::shared_memory
		: public shared_memory_base, public ooe::memory
	{
		enum type
		{
			open,
			create
		};

		shared_memory( const std::string&, type = open, up_t = 0 );
		shared_memory( const std::string&, const ooe::descriptor& );
		~shared_memory( void );
	};

//--- ipc::locked_memory ---------------------------------------------------------
	struct OOE_VISIBLE ipc::locked_memory
		: public shared_memory
	{
		locked_memory( const std::string&, type = open, up_t = 0 );
		~locked_memory( void );
	};

//--- ipc::memory_lock ---------------------------------------------------------
	class OOE_VISIBLE ipc::memory_lock
		: private noncopyable
	{
	public:
		memory_lock( shared_memory& );
		~memory_lock( void );

	private:
		shared_memory& memory;
	};
}

#endif	// OOE_FOUNDATION_IPC_SHARED_MEMORY_HPP
