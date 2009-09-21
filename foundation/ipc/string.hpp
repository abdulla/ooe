/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_JUMBO_HPP
#define OOE_FOUNDATION_IPC_MEMORY_JUMBO_HPP

#include "foundation/ipc/traits.hpp"

namespace ooe
{
	namespace ipc
	{
		class string;

		template< typename >
			struct is_ipcstring;

		template< typename type >
			struct size< type, typename enable_if< is_ipcstring< type > >::type >;

		template< typename type >
			struct read< type, typename enable_if< is_ipcstring< type > >::type >;

		template< typename type >
			struct write< type, typename enable_if< is_ipcstring< type > >::type >;

		namespace memory
		{
			template< typename >
				struct invoke_function;

			template< typename, typename >
				struct invoke_member;

			template< typename >
				struct rpc;
		}

		namespace socket
		{
			template< typename >
				struct invoke_function;

			template< typename, typename >
				struct invoke_member;

			template< typename >
				struct rpc;
		}
	}

//--- ipc::string --------------------------------------------------------------
	class ipc::string
	{
	public:
		string( const std::string& stdstring_ )
			: stdstring( &stdstring_ ), size_( -1 )
		{
		}

		string( const c8* cstring_ )
			: cstring( cstring_ ), size_( std::strlen( cstring ) )
		{
		}

		string& operator =( const std::string& stdstring_ )
		{
			stdstring = &stdstring_;
			size_ = -1;
			return *this;
		}

		string& operator =( const c8* cstring_ )
		{
			cstring = cstring_;
			size_ = std::strlen( cstring );
			return *this;
		}

		const c8* c_str( void ) const
		{
			return size_ == up_t( -1 ) ? stdstring->c_str() : cstring;
		}

		up_t size( void ) const
		{
			return size_ == up_t( -1 ) ? stdstring->size() : size_;
		}

	private:
		union
		{
			const c8* cstring;
			const std::string* stdstring;
		};

		up_t size_;

		string( void )
			: cstring( 0 ), size_( 0 )
		{
		}

		template< typename >
			friend struct memory::invoke_function;

		template< typename, typename >
			friend struct memory::invoke_member;

		template< typename >
			friend struct memory::rpc;

		template< typename >
			friend struct socket::invoke_function;

		template< typename, typename >
			friend struct socket::invoke_member;

		template< typename >
			friend struct socket::rpc;
	};

//--- ipc::is_ipcstring --------------------------------------------------------
	template< typename type >
		struct ipc::is_ipcstring
		: public is_like< type, ipc::string >
	{
	};

//--- ipc::traits: ipc::string -------------------------------------------------
	template< typename type >
		struct ipc::size< type, typename enable_if< ipc::is_ipcstring< type > >::type >
	{
		static up_t call( const ipc::string& string ) OOE_PURE
		{
			return string.size() + 1;
		}
	};

	template< typename type >
		struct ipc::read< type, typename enable_if< ipc::is_ipcstring< type > >::type >
	{
		static up_t call( const u8* buffer, ipc::string& string )
		{
			string = reinterpret_cast< const c8* >( buffer );
			return size< type >::call( string );
		}
	};

	template< typename type >
		struct ipc::write< type, typename enable_if< ipc::is_ipcstring< type > >::type >
	{
		static up_t call( u8* buffer, const ipc::string& string )
		{
			up_t size = ipc::size< type >::call( string );
			std::memcpy( buffer, string.c_str(), size );
			return size;
		}
	};

}

#endif	// OOE_FOUNDATION_IPC_JUMBO_HPP
