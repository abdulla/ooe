/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_ALLOCATOR_HPP
#define OOE_FOUNDATION_IPC_ALLOCATOR_HPP

#include <vector>

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "foundation/ipc/traits.hpp"

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
			struct is_ipccontainer< ipc::vector< type > >;

		template< typename type >
			struct is_container;

		template< typename type >
			struct replace< type, typename enable_if< ipc::is_container< type > >::type >;

		template< typename type >
			struct pack< type, typename enable_if< ipc::is_container< type > >::type >;

		template< typename type >
			struct unpack< type, typename enable_if< ipc::is_container< type > >::type >;

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
			return memory ? memory->name() : std::string();
		}

		storage_type storage( void ) const
		{
			pointer start = memory->as< type >();
			return storage_type( start, start + used, start + memory->size() / sizeof( type ) );
		}

		size_type max_size( void ) const
		{
			return static_cast< size_type >( -1 ) / sizeof( type );
		}

		pointer allocate( size_type size, std::allocator< void >::const_pointer = 0 )
		{
			if ( !memory || memory->size() < size )
			{
				reserve = memory;
				memory = new shared_memory( unique_name(), shared_memory::create, size );
			}

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
	template< typename type >
		struct ipc::is_ipccontainer
		: public false_type
	{
	};

	template< typename type >
		struct ipc::is_ipccontainer< ipc::vector< type > >
		: public true_type
	{
	};

//--- ipc::is_container --------------------------------------------------------
	template< typename type >
		struct ipc::is_container
		: public is_ipccontainer< typename no_ref< type >::type >
	{
	};

//--- ipc::traits: container ---------------------------------------------------
	template< typename t >
		struct ipc::replace< t, typename enable_if< ipc::is_container< t > >::type >
	{
		typedef tuple< std::string, up_t > value_type;
		typedef typename replace< value_type >::type type;
	};

	template< typename t >
		struct ipc::pack< t, typename enable_if< ipc::is_container< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( const type& in, value_type& out, buffer_pack& buffer_pack )
		{
			typedef typename replace< t >::value_type contained_type;
			contained_type value( in.get_allocator().name(), in.size() );
			pack< contained_type >::call( value, out, buffer_pack );
		}
	};

	template< typename t >
		struct ipc::unpack< t, typename enable_if< ipc::is_container< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( const value_type& in, type& out, const buffer_unpack& buffer_unpack )
		{
			typedef typename replace< t >::value_type contained_type;
			contained_type value;
			unpack< contained_type >::call( in, value, buffer_unpack );

			typedef typename type::allocator_type allocator_type;
			out = type( allocator_type( value._0, value._1 ) );
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_ALLOCATOR_HPP
