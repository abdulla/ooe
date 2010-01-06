/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_CONTAINER_HPP
#define OOE_FOUNDATION_IPC_CONTAINER_HPP

#include <vector>

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "foundation/ipc/traits.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace ipc
	{
		template< typename, typename = void >
			class allocator;

		template< typename type >
			class allocator< type, typename enable_if< has_trivial_copy< type > >::type >;

		template< typename >
			class vector;

		template< typename >
			struct is_ipccontainer;

		template< typename type >
			struct size< type, typename enable_if< ipc::is_ipccontainer< type > >::type >;

		template< typename type >
			struct read< type, typename enable_if< ipc::is_ipccontainer< type > >::type >;

		template< typename type >
			struct write< type, typename enable_if< ipc::is_ipccontainer< type > >::type >;

		template< typename type >
			bool operator ==( const allocator< type >& x, const allocator< type >& y );

		template< typename type >
			bool operator !=( const allocator< type >& x, const allocator< type >& y );
	}

//--- ipc::allocator -----------------------------------------------------------
	template< typename type >
		class ipc::allocator< type, typename enable_if< has_trivial_copy< type > >::type >
	{
	public:
		typedef type value_type;
		typedef up_t size_type;
		typedef sp_t difference_type;

		typedef type* pointer;
		typedef const type* const_pointer;
		typedef type& reference;
		typedef const type& const_reference;

		typedef tuple< pointer, pointer, pointer > storage_type;

		template< typename t >
			struct rebind
		{
			typedef allocator< t > other;
		};

		allocator( void )
			: used( 0 ), memory(), reserve()
		{
		}

		allocator( const std::string& name_, size_type used_ )
			: used( used_ ), memory( new shared_memory( name_ ) ), reserve()
		{
		}

		std::string name( void ) const
		{
			if ( !memory )
				throw error::runtime( "ipc::allocator: " ) << "Empty, no name available";

			return memory->name();
		}

		storage_type storage( void ) const
		{
			pointer start = memory->as< type >();
			return storage_type( start, start + used, start + memory->size() / sizeof( type ) );
		}

		size_type max_size( void ) const
		{
			return ~size_type( 0 ) / sizeof( type );
		}

		pointer allocate( size_type size, std::allocator< void >::const_pointer = 0 )
		{
			reserve = memory;
			memory = new shared_memory( unique_name(), shared_memory::create, size );
			return memory->as< type >();
		}

		void deallocate( pointer p, size_type )
		{
			if ( memory && p == memory->as< type >() )
				memory = 0;
			else if ( reserve && p == reserve->as< type >() )
				reserve = 0;
			else
				throw error::runtime( "ipc::allocator: " ) << "Invalid pointer " << p;
		}

		void construct( pointer p, const type& v )
		{
			new( p ) value_type( v );
		}

		void destroy( pointer p )
		{
			p->~type();
		}

	private:
		size_type used;
		shared_ptr< shared_memory > memory;
		shared_ptr< shared_memory > reserve;
	};

	template< typename type >
		bool ipc::operator ==( const allocator< type >& x, const allocator< type >& y )
	{
		return x.name() == y.name();
	}

	template< typename type >
		bool ipc::operator !=( const allocator< type >& x, const allocator< type >& y )
	{
		return x.name() != y.name();
	}

//--- ipc::vector --------------------------------------------------------------
	template< typename type >
		class ipc::vector
		: public std::vector< type, ipc::allocator< type > >
	{
	public:
		typedef std::vector< type, ipc::allocator< type > > base_type;
		typedef typename base_type::allocator_type allocator_type;

		vector( void )
			: base_type()
		{
		}

		vector( const allocator_type& a )
			: base_type( a )
		{
			assign( a );
		}

		vector( const vector& v )
			: base_type( v.get_allocator() )
		{
			assign( _M_impl );
		}

		vector& operator =( const allocator_type& a )
		{
			_M_impl = a;
			assign( a );
			return *this;
		}

		vector& operator =( const vector& v )
		{
			_M_impl = v.get_allocator();
			assign( _M_impl );
			return *this;
		}

	private:
		using base_type::_M_impl;

		void assign( const allocator_type& a )
		{
			typename allocator_type::storage_type storage = a.storage();
			_M_impl._M_start = storage._0;
			_M_impl._M_finish = storage._1;
			_M_impl._M_end_of_storage = storage._2;
		}
	};

//--- ipc::is_ipccontainer -----------------------------------------------------
	template< typename t >
		struct ipc::is_ipccontainer
	{
		template< typename >
			struct apply
			: public false_type
		{
		};

		template< typename type >
			struct apply< vector< type > >
			: public true_type
		{
		};

		static const bool value = apply< typename no_ref< t >::type >::value;
	};

//--- ipc::traits: container ---------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if< ipc::is_ipccontainer< t > >::type >
	{
		static up_t call( typename call_traits< t >::param_type value ) OOE_PURE
		{
			typedef tuple< std::string, up_t > tuple_type;
			tuple_type tuple( value.get_allocator().name(), value.size() );
			return size< tuple_type >::call( tuple );
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if< ipc::is_ipccontainer< t > >::type >
	{
		static up_t call( const u8* buffer, typename call_traits< t >::reference value )
		{
			typedef tuple< std::string, up_t > tuple_type;
			tuple_type tuple;
			up_t size = read< tuple_type >::call( buffer, tuple );

			value = typename no_ref< t >::type::allocator_type( tuple._0, tuple._1 );
			return size;
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if< ipc::is_ipccontainer< t > >::type >
	{
		static up_t call( u8* buffer, typename call_traits< t >::param_type value )
		{
			typedef tuple< std::string, up_t > tuple_type;
			tuple_type tuple( value.get_allocator().name(), value.size() );
			return write< tuple_type >::call( buffer, tuple );
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_CONTAINER_HPP
