/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_MEMORY_TRAITS_HPP
	#define OOE_FOUNDATION_IPC_MEMORY_TRAITS_HPP

#include <boost/mpl/transform.hpp>

#include "foundation/utility/error.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/traits.hpp"
#include "foundation/utility/tuple.hpp"

#include "foundation/ipc/buffer.hpp"
#include "foundation/ipc/traits_forward.hpp"

namespace ooe
{
	namespace ipc
	{
//--- ipc::replace -------------------------------------------------------------
		template< typename type >
			struct replace< type, typename enable_if< is_tuple< type > >::type >;

		template< typename type >
			struct replace< type, typename enable_if< ooe::is_container< type > >::type >;

		template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, no_t ) >
			struct layout_replace;

//--- ipc::pack ----------------------------------------------------------------
		template< typename type >
			struct pack< type, typename enable_if< ooe::is_container< type > >::type >;

		template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, no_t ) >
			struct layout_pack;

//--- ipc::unpack --------------------------------------------------------------
		template< typename type >
			struct unpack< type, typename enable_if< ooe::is_container< type > >::type >;

		template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, no_t ) >
			struct layout_unpack;

//--- ipc::reserve -------------------------------------------------------------
		template< typename >
			struct reserve;

		template< typename t >
			struct reserve< std::vector< t, std::allocator< t > > >;
	}

//--- ipc::traits: tuple -------------------------------------------------------
	template< typename t >
		struct ipc::replace< t, typename enable_if< is_tuple< t > >::type >
	{
		typedef typename no_ref< t >::type t0;
		typedef boost::mpl::placeholders::_ _;
		typedef typename boost::mpl::transform< t0, replace< _ > >::type type;
	};

//--- ipc::traits: container ---------------------------------------------------
	template< typename t >
		struct ipc::replace< t, typename enable_if< ooe::is_container< t > >::type >
	{
		typedef struct { bool external : 1; unsigned size : 31; unsigned offset : 32; } type;
	};

	template< typename t >
		struct ipc::pack< t, typename enable_if< ooe::is_container< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( const type& in, value_type& out, buffer_pack& buffer_pack )
		{
			typedef typename type::value_type contained_type;
			typedef typename type::const_iterator iterator_type;
			typedef typename replace< contained_type >::type replaced_type;

			up_t in_size = in.size();
			buffer_pack::allocate_tuple allocate =
				buffer_pack.allocate( sizeof( replaced_type ) * in_size );

			if ( in_size > 0x7fffffff )
				error::runtime( "ipc::pack: " ) << "Unable to encode size " << in_size;
			else if ( allocate._1 > 0xffffffff )
				error::runtime( "ipc::pack: " ) << "Unable to encode offset " << allocate._1;

			replaced_type* pointer = reinterpret_cast< replaced_type* >( allocate._0 );
			out.offset = allocate._1;
			out.size = in_size;
			out.external = allocate._2;

			for ( iterator_type i = in.begin(), end = in.end(); i != end; ++i, ++pointer )
				pack< contained_type >::call( *i, *pointer, buffer_pack );
		}
	};

	template< typename t >
		struct ipc::unpack< t, typename enable_if< ooe::is_container< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( const value_type& in, type& out, const buffer_unpack& buffer_unpack )
		{
			typedef typename type::value_type contained_type;
			typedef typename replace< contained_type >::type replaced_type;

			const replaced_type* pointer = add< const replaced_type >
				( in.external ? buffer_unpack.external : buffer_unpack.internal, in.offset );
			reserve< type >::call( out, in.size );

			for ( up_t i = 0; i != in.size; ++i, ++pointer )
			{
				contained_type value;
				unpack< contained_type >::call( *pointer, value, buffer_unpack );
				out.insert( out.end(), value );
			}
		}
	};

//--- ipc::reserve -------------------------------------------------------------
	template< typename t >
		struct ipc::reserve
	{
		static void call( typename call_traits< t >::param_type, up_t )
		{
		}
	};

	template< typename t >
		struct ipc::reserve< std::vector< t, std::allocator< t > > >
	{
		static void call( std::vector< t, std::allocator< t > >& vector, up_t size )
		{
			vector.reserve( size );
		}
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/memory/traits.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_MEMORY_TRAITS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define PACK( z, n, _ ) pack< t ## n >::call( a ## n, t._ ## n, buffer_pack );
	#define UNPACK( z, n, _ ) unpack< typename no_ref< t ## n >::type >::\
		call( t._ ## n, a ## n, buffer_unpack );
	#define TUPLE_PACK( z, n, _ ) pack< typename tuple_element< n, type >::type >::\
		call( at< n >( in ), at< n >( out ), buffer_pack );
	#define TUPLE_UNPACK( z, n, _ ) unpack< typename tuple_element< n, type >::type >::\
		call( at< n >( in ), at< n >( out ), buffer_unpack );

namespace ooe
{
	namespace ipc
	{
//--- ipc::replace -------------------------------------------------------------
		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
			struct layout_replace;
#else
			struct layout_replace< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >;
#endif

//--- ipc::pack ----------------------------------------------------------------
		template< typename t >
			struct pack< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >;

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
			struct layout_pack;
#else
			struct layout_pack< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >;
#endif

//--- ipc::unpack --------------------------------------------------------------
		template< typename t >
			struct unpack< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >;

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
			struct layout_unpack;
#else
			struct layout_unpack< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >;
#endif
	}

//--- ipc::traits: tuple -------------------------------------------------------
	template< typename t >
		struct ipc::pack< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

#if LIMIT
		static void call( const type& in, value_type& out, buffer_pack& buffer_pack )
#else
		static void call( const type&, value_type&, buffer_pack& )
#endif
		{
			BOOST_PP_REPEAT( LIMIT, TUPLE_PACK, _ )
		}
	};

	template< typename t >
		struct ipc::unpack< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

#if LIMIT
		static void call( const value_type& in, type& out, const buffer_unpack& buffer_unpack )
#else
		static void call( const value_type&, type&, const buffer_unpack& buffer )
#endif
		{
			BOOST_PP_REPEAT( LIMIT, TUPLE_UNPACK, _ )
		}
	};

//--- ipc::layout_replace ------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
		struct ipc::layout_replace
#else
		struct ipc::layout_replace< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >
#endif
	{
		typedef tuple< BOOST_PP_ENUM_BINARY_PARAMS
			( LIMIT, typename replace< t, >::type BOOST_PP_INTERCEPT ) > type;
	};

//--- ipc::layout_pack ---------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
		struct ipc::layout_pack
#else
		struct ipc::layout_pack< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >
#endif
	{
		static void call( buffer_pack& buffer_pack BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) )
		{
			typedef BOOST_PP_EXPR_IF( LIMIT, typename )
				layout_replace< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::type type;
			buffer_pack::allocate_tuple allocate = buffer_pack.allocate( sizeof( type ) );

			if ( allocate._2 )
				throw error::runtime( "ipc::layout_pack: " ) << "Overflowed internal buffer";

			BOOST_PP_EXPR_IF( LIMIT, type& t = *reinterpret_cast< type* >( allocate._0 ); )
			BOOST_PP_REPEAT( LIMIT, PACK, _ )
		}
	};

//--- ipc::layout_unpack -------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
		struct ipc::layout_unpack
#else
		struct ipc::layout_unpack< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >
#endif
	{
		static void call( const buffer_unpack& BOOST_PP_EXPR_IF( LIMIT, buffer_unpack )
			BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, typename no_ref< t, >::type& a ) )
		{
			typedef BOOST_PP_EXPR_IF( LIMIT, typename ) layout_replace< BOOST_PP_ENUM_BINARY_PARAMS
				( LIMIT, typename no_ref< t, >::type BOOST_PP_INTERCEPT ) >::type type;
			BOOST_PP_EXPR_IF( LIMIT,
				const type& t = *reinterpret_cast< type* >( buffer_unpack.internal ); )
			BOOST_PP_REPEAT( LIMIT, UNPACK, _ )
		}
	};
}

	#undef TUPLE_UNPACK
	#undef TUPLE_PACK
	#undef UNPACK
	#undef PACK
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
