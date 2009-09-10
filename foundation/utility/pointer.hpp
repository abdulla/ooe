/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_POINTER_HPP
#define OOE_FOUNDATION_UTILITY_POINTER_HPP

#include "foundation/utility/noncopyable.hpp"

#ifndef __THROW
#define __THROW
#endif

extern "C" void free( void* ) __THROW;

namespace ooe
{
//--- deallocate_ptr -----------------------------------------------------------
	template< typename type >
		struct deallocate_ptr
	{
		static void call( type* value )
		{
			delete value;
		}
	};

//--- deallocate_array ---------------------------------------------------------
	template< typename type >
		struct deallocate_array
	{
		static void call( type* value )
		{
			delete[] value;
		}
	};

//--- deallocate_free ----------------------------------------------------------
	template< typename type >
		struct deallocate_free
	{
		static void call( type* value )
		{
			free( value );
		}
	};

//--- linked_ptr ---------------------------------------------------------------

//--- scoped_base --------------------------------------------------------------
	template< typename type, typename deleter >
		class scoped_base
		: private noncopyable
	{
	public:
		operator type*( void ) const
		{
			return value;
		}

		type* operator ->( void ) const
		{
			return value;
		}

		type* get( void ) const
		{
			return value;
		}

		template< typename to >
			to* as( void ) const
		{
			return reinterpret_cast< to* >( value );
		}

		type* release( void )
		{
			type* pass_back = value;
			value = 0;
			return pass_back;
		}

		void swap( scoped_base& exchange )
		{
			type* save = value;
			value = exchange.value;
			exchange.value = save;
		}

	protected:
		type* value;

		scoped_base( type* value_ )
			: value( value_ )
		{
		}

		~scoped_base( void )
		{
			deleter::call( value );
		}
	};

//--- scoped_dereference -------------------------------------------------------
	template< typename type, typename deleter >
		class scoped_dereference
		: public scoped_base< type, deleter >
	{
	public:
		type& operator *( void ) const
		{
			return *this->value;
		}

	protected:
		scoped_dereference( type* value_ )
			: scoped_base< type, deleter >( value_ )
		{
		}
	};

	template< typename deleter >
		class scoped_dereference< void, deleter >
		: public scoped_base< void, deleter >
	{
	protected:
		scoped_dereference( void* value_ )
			: scoped_base< void, deleter >( value_ )
		{
		}
	};

//--- scoped_ptr ---------------------------------------------------------------
	template< typename type, template< typename > class deleter = deallocate_ptr >
		struct scoped_ptr
		: public scoped_dereference< type, deleter< type > >
	{
		scoped_ptr( type* value_ )
			: scoped_dereference< type, deleter< type > >( value_ )
		{
		}
	};

//--- scoped_array -------------------------------------------------------------
	template< typename type, template< typename > class deleter = deallocate_array >
		struct scoped_array
		: public scoped_dereference< type, deleter< type > >
	{
		scoped_array( type* value_ )
			: scoped_dereference< type, deleter< type > >( value_ )
		{
		}
	};

//--- scoped_free --------------------------------------------------------------
	template< typename type, template< typename > class deleter = deallocate_free >
		struct scoped_free
		: public scoped_dereference< type, deleter< type > >
	{
		scoped_free( type* value_ )
			: scoped_dereference< type, deleter< type > >( value_ )
		{
		}
	};

//--- shared_ref -------------------------------------------------------------------
	template< typename type, typename deleter, typename ref_t >
		class shared_ref
	{
	public:
		shared_ref( type* value_ )
			: refs( 1 ), value( value_ )
		{
		}

		void increment( void )
		{
			++refs;
		}

		void decrement( void )
		{
			if ( --refs )
				return;

			deleter::call( value );
			delete this;
		}

		type* get( void ) const
		{
			return value;
		}

	private:
		ref_t refs;
		type* const value;
	};

//--- shared_base --------------------------------------------------------------
	template< typename type, typename deleter, typename ref_t >
		class shared_base
	{
	public:
		shared_base( const shared_base& copy )
			: ref( copy.ref )
		{
			ref->increment();
		}

		shared_base& operator =( const shared_base& copy )
		{
			copy.ref->increment();
			ref->decrement();
			ref = copy.ref;
			return *this;
		}

		operator type*( void ) const
		{
			return ref->get();
		}

		type* operator ->( void ) const
		{
			return ref->get();
		}

		type* get( void ) const
		{
			return ref->get();
		}

		template< typename to >
			to* as( void ) const
		{
			return reinterpret_cast< to* >( ref->get() );
		}

		void swap( shared_base& exchange )
		{
			ref_type* save = ref;
			ref = exchange.ref;
			exchange.ref = save;
		}

	protected:
		typedef shared_ref< type, deleter, ref_t > ref_type;
		ref_type* ref;

		shared_base( type* value )
			: ref( new ref_type( value ) )
		{
		}

		~shared_base( void )
		{
			ref->decrement();
		}
	};

//--- shared_dereference -------------------------------------------------------
	template< typename type, typename deleter, typename ref_t >
		class shared_dereference
		: public shared_base< type, deleter, ref_t >
	{
	public:
		type& operator *( void ) const
		{
			return *this->ref->get();
		}

	protected:
		shared_dereference( type* value )
			: shared_base< type, deleter, ref_t >( value )
		{
		}
	};

	template< typename deleter, typename ref_t >
		class shared_dereference< void, deleter, ref_t >
		: public shared_base< void, deleter, ref_t >
	{
	protected:
		shared_dereference( void* value )
			: shared_base< void, deleter, ref_t >( value )
		{
		}
	};

//--- shared_ptr ---------------------------------------------------------------
	template< typename type, template< typename > class deleter = deallocate_ptr >
		struct shared_ptr
		: public shared_dereference< type, deleter< type >, unsigned >
	{
		shared_ptr( type* value = 0 )
			: shared_dereference< type, deleter< type >, unsigned >( value )
		{
		}
	};

//--- shared_array -------------------------------------------------------------
	template< typename type, template< typename > class deleter = deallocate_array >
		struct shared_array
		: public shared_dereference< type, deleter< type >, unsigned >
	{
		shared_array( type* value = 0 )
			: shared_dereference< type, deleter< type >, unsigned >( value )
		{
		}
	};

//--- shared_free --------------------------------------------------------------
	template< typename type, template< typename > class deleter = deallocate_free >
		struct shared_free
		: public shared_dereference< type, deleter< type >, unsigned >
	{
		shared_free( type* value = 0 )
			: shared_dereference< type, deleter< type >, unsigned >( value )
		{
		}
	};
}

#endif	// OOE_FOUNDATION_UTILITY_POINTER_HPP
