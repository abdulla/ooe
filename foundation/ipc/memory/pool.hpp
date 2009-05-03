/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_POOL_HPP
#define OOE_FOUNDATION_IPC_MEMORY_POOL_HPP

#include <map>

#include "foundation/ipc/traits.hpp"

namespace ooe
{
	namespace ipc
	{
		class pool;

//--- *_ptr --------------------------------------------------------------------
		template< typename >
			class proxy_ptr;

		template< typename >
			struct construct_ptr;

		template< typename >
			struct destruct_ptr;

		template< typename >
			struct nullable_ptr;

		template< typename >
			struct unchecked_ptr;

//--- is_* ---------------------------------------------------------------------
		template< typename >
			struct is_pointer;

		template< typename >
			struct is_construct;

		template< typename >
			struct is_destruct;

		template< typename >
			struct is_nullable;

		template< typename >
			struct is_unchecked;

		template< typename >
			struct is_proxy;

//--- ipc::traits: proxy -------------------------------------------------------
		template< typename type >
			struct replace< type, typename enable_if< is_proxy< type > >::type >;

		template< typename type >
			struct pack< type, typename enable_if< is_proxy< type > >::type >;

		template< typename type >
			struct unpack< type, typename enable_if< is_proxy< type > >::type >;

//--- verify -------------------------------------------------------------------
		template< typename, typename = void >
			struct verify;

		template< typename type >
			struct verify< type, typename enable_if< is_construct< type > >::type >;

		template< typename type >
			struct verify< type, typename enable_if< is_destruct< type > >::type >;

		template< typename type >
			struct verify< type, typename enable_if< is_pointer< type > >::type >;

		template< typename type >
			struct verify< type, typename enable_if< is_nullable< type > >::type >;
	}

//--- ipc::pool ----------------------------------------------------------------
	class ipc::pool
		: private noncopyable
	{
	public:
		typedef void ( * function_type )( void* );

		~pool( void );

		void insert( void*, function_type ) OOE_VISIBLE;
		void erase( void* ) OOE_VISIBLE;
		bool find( void* ) const OOE_VISIBLE;

	private:
		typedef std::map< void*, function_type > map_type;

		map_type map;
	};

//--- ipc::proxy_ptr -----------------------------------------------------------
	template< typename type >
		class ipc::proxy_ptr
	{
	public:
		typedef type* pointer;
		typedef type& reference;

		operator pointer( void ) const
		{
			return value;
		}

		pointer operator ->( void ) const
		{
			return value;
		}

		reference operator *( void ) const
		{
			return *value;
		}

	protected:
		proxy_ptr( pointer value_ )
			: value( value_ )
		{
		}

	private:
		pointer value;
	};

//--- ipc::construct_ptr -------------------------------------------------------
	template< typename type >
		struct ipc::construct_ptr
		: public proxy_ptr< type >
	{
		construct_ptr( type* value_ = 0 )
			: proxy_ptr< type >( value_ )
		{
		}
	};

//--- ipc::destruct_ptr --------------------------------------------------------
	template< typename type >
		struct ipc::destruct_ptr
		: public proxy_ptr< type >
	{
		destruct_ptr( type* value_ = 0 )
			: proxy_ptr< type >( value_ )
		{
		}
	};

//--- ipc::nullable_ptr --------------------------------------------------------
	template< typename type >
		struct ipc::nullable_ptr
		: public proxy_ptr< type >
	{
		nullable_ptr( type* value_ = 0 )
			: proxy_ptr< type >( value_ )
		{
		}
	};

//--- ipc::unchecked_ptr -------------------------------------------------------
	template< typename type >
		struct ipc::unchecked_ptr
		: public proxy_ptr< type >
	{
		unchecked_ptr( type* value_ = 0 )
			: proxy_ptr< type >( value_ )
		{
		}
	};

//--- ipc::is_pointer ----------------------------------------------------------
	template< typename type >
		struct ipc::is_pointer
	{
		static const bool value = !is_cstring< type >::value && ooe::is_pointer< type >::value;
	};

//--- ipc::is_construct --------------------------------------------------------
	template< typename type >
		struct ipc::is_construct
#if __GNUC__ >=4 && __GNUC_MINOR__ >= 2 && __GNUC_PATCHLEVEL__ >= 4
		: public is_template1< type, construct_ptr >
	{
	};
#else
	{
		template< typename >
			struct apply
			: public false_type
		{
		};

		template< typename t >
			struct apply< construct_ptr< t > >
			: public true_type
		{
		};

		static const bool value = apply< typename no_ref< type >::type >::value;
	};
#endif

//--- ipc::is_destruct ---------------------------------------------------------
	template< typename type >
		struct ipc::is_destruct
#if __GNUC__ >=4 && __GNUC_MINOR__ >= 2 && __GNUC_PATCHLEVEL__ >= 4
		: public is_template1< type, destruct_ptr >
	{
	};
#else
	{
		template< typename >
			struct apply
			: public false_type
		{
		};

		template< typename t >
			struct apply< destruct_ptr< t > >
			: public true_type
		{
		};

		static const bool value = apply< typename no_ref< type >::type >::value;
	};
#endif

//--- ipc::is_nullable ---------------------------------------------------------
	template< typename type >
		struct ipc::is_nullable
#if __GNUC__ >=4 && __GNUC_MINOR__ >= 2 && __GNUC_PATCHLEVEL__ >= 4
		: public is_template1< type, nullable_ptr >
	{
	};
#else
	{
		template< typename >
			struct apply
			: public false_type
		{
		};

		template< typename t >
			struct apply< nullable_ptr< t > >
			: public true_type
		{
		};

		static const bool value = apply< typename no_ref< type >::type >::value;
	};
#endif

//--- ipc::is_unchecked --------------------------------------------------------
	template< typename type >
		struct ipc::is_unchecked
#if __GNUC__ >=4 && __GNUC_MINOR__ >= 2 && __GNUC_PATCHLEVEL__ >= 4
		: public is_template1< type, unchecked_ptr >
	{
	};
#else
	{
		template< typename >
			struct apply
			: public false_type
		{
		};

		template< typename t >
			struct apply< unchecked_ptr< t > >
			: public true_type
		{
		};

		static const bool value = apply< typename no_ref< type >::type >::value;
	};
#endif

//--- ipc::is_proxy ------------------------------------------------------------
	template< typename type >
		struct ipc::is_proxy
	{
		static const bool value = is_construct< type >::value || is_destruct< type >::value ||
			is_nullable< type >::value || is_unchecked< type >::value;
	};

//--- ipc::traits: proxy -------------------------------------------------------
	template< typename t >
		struct ipc::replace< t, typename enable_if< ipc::is_proxy< t > >::type >
	{
		typedef typename no_ref< t >::type::pointer type;
	};

	template< typename t >
		struct ipc::pack< t, typename enable_if< ipc::is_proxy< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( type in, value_type& out, buffer_pack& )
		{
			out = in;
		}
	};

	template< typename t >
		struct ipc::unpack< t, typename enable_if< ipc::is_proxy< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( value_type in, type& out, const buffer_unpack& )
		{
			out = in;
		}
	};

//--- ipc::verify --------------------------------------------------------------
	template< typename type, typename >
		struct ipc::verify
	{
		static void call( pool&, typename call_traits< type >::param_type, u8 )
		{
		}
	};

	template< typename type >
		struct ipc::verify< type, typename enable_if< ipc::is_construct< type > >::type >
	{
		static void call( pool& pool, type pointer, u8 )
		{
			typedef typename remove_pointer< typename type::pointer >::type value_type;
			pool.insert( pointer, destroy< value_type > );
		}
	};

	template< typename type >
		struct ipc::verify< type, typename enable_if< ipc::is_destruct< type > >::type >
	{
		static void call( pool& pool, type pointer, u8 )
		{
			pool.erase( pointer );
		}
	};

	template< typename type >
		struct ipc::verify< type, typename enable_if< ipc::is_pointer< type > >::type >
	{
		static void call( pool& pool, type pointer, u8 index )
		{
			// could speed up further by checking against known out-of-bounds ranges/values
			if ( !pool.find( pointer ) )
				throw error::runtime( "ipc::verify: " ) <<
					"Invalid pointer " << pointer << " in argument " << index;
		}
	};

	template< typename type >
		struct ipc::verify< type, typename enable_if< ipc::is_nullable< type > >::type >
	{
		static void call( pool& pool, type pointer, u8 index )
		{
			if ( pointer && !pool.find( pointer ) )
				throw error::runtime( "ipc::verify: " ) <<
					"Invalid pointer " << pointer << " in argument " << index;
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_POOL_HPP
