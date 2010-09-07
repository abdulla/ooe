/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_UTILITY_TUPLE_HPP
	#define OOE_FOUNDATION_UTILITY_TUPLE_HPP

#include <utility>

#include <boost/mpl/at.hpp>
#include <boost/mpl/clear.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/iterator_tags.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/size.hpp>

#include "foundation/utility/preprocessor.hpp"
#include "foundation/utility/traits.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct void_t;
struct tuple_tag;

template< unsigned, typename, bool >
	struct tuple_base;

template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, void_t ) >
	struct tuple;

template< typename, typename, typename = void >
	struct tuple_push_front;

template< typename, typename, typename = void >
	struct tuple_push_back;

template< typename, typename = void >
	struct tuple_size;

template< unsigned, typename >
	struct tuple_element;

template< unsigned, typename, typename >
	struct tuple_equal;

template< unsigned, typename, typename >
	struct tuple_less;

template< unsigned, typename, typename = void >
	struct tuple_at;

//--- at -------------------------------------------------------------------------------------------
template< unsigned i, typename t >
	typename tuple_at< i, t >::type at( t& tuple )
{
	return tuple_at< i, t >::call( tuple );
}

//--- is_tuple -------------------------------------------------------------------------------------
template< typename type >
	struct is_tuple
#if __GNUC__ >=4 && __GNUC_MINOR__ >= 2 && __GNUC_PATCHLEVEL__ >= 4
	: public BOOST_PP_CAT( is_template, OOE_PP_LIMIT )< type, tuple >
{
};
#else	// still needed due to objective-c compilation with gcc 4.2.1
{
	template< typename >
		struct apply
		: public false_type
	{
	};

	template< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, typename t ) >
		struct apply< tuple< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, t ) > >
		: public true_type
	{
	};

	static const bool value = apply< typename no_ref< type >::type >::value;
};
#endif

//--- tuple_iterator -------------------------------------------------------------------------------
template< typename, typename >
	struct tuple_iterator
{
	typedef boost::mpl::random_access_iterator_tag category;
};

//--- tuple_size -----------------------------------------------------------------------------------
template< typename type >
	struct tuple_size< type, typename enable_if< is_tuple< type > >::type >
{
	static const unsigned value = no_ref< type >::type::size;
};

//--- tuple_equal ----------------------------------------------------------------------------------
template< typename a, typename b >
	struct tuple_equal< 0, a, b >
{
	static bool call( const a&, const b& )
	{
		return true;
	}
};

//--- tuple_less -----------------------------------------------------------------------------------
template< typename a, typename b >
	struct tuple_less< 0, a, b >
{
	static bool call( const a& x, const b& y )
	{
		return at< 0 >( x ) < at< 0 >( y );
	}
};

//--- operator == ----------------------------------------------------------------------------------
template< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, typename s )
	BOOST_PP_ENUM_TRAILING_PARAMS( OOE_PP_LIMIT, typename t ) >
	bool operator ==( const tuple< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, s ) >& x,
	const tuple< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, t ) >& y )
{
	typedef tuple< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, s ) > a;
	typedef tuple< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, t ) > b;
	return tuple_equal< tuple_size< a >::value, a, b >::call( x, y );
}

//--- operator < -----------------------------------------------------------------------------------
template< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, typename s )
	BOOST_PP_ENUM_TRAILING_PARAMS( OOE_PP_LIMIT, typename t ) >
	bool operator <( const tuple< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, s ) >& x,
	const tuple< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, t ) >& y )
{
	typedef tuple< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, s ) > a;
	typedef tuple< BOOST_PP_ENUM_PARAMS( OOE_PP_LIMIT, t ) > b;
	return tuple_less< tuple_size< a >::value - 1, a, b >::call( x, y );
}

//--- tuple_print ----------------------------------------------------------------------------------
template< unsigned i >
	struct tuple_print
{
	template< typename t0, typename t1 >
		static void call( t0& out, const t1& in )
	{
		tuple_print< i - 1 >::call( out, in );

		if ( i > 1 )
			out << ", ";

		out << at< i - 1 >( in );
	}
};

template<>
	struct tuple_print< 0 >
{
	template< typename t0, typename t1 >
		static void call( t0&, const t1& )
	{
	}
};

OOE_NAMESPACE_END( ( ooe ) )

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/utility/tuple.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

OOE_NAMESPACE_BEGIN( ( boost )( mpl ) )

//--- next -----------------------------------------------------------------------------------------
template< typename t, typename i >
	struct next< ooe::tuple_iterator< t, i > >
{
	typedef ooe::tuple_iterator< t, typename next< i >::type > type;
};

//--- prior ----------------------------------------------------------------------------------------
template< typename t, typename i >
	struct prior< ooe::tuple_iterator< t, i > >
{
	typedef ooe::tuple_iterator< t, typename prior< i >::type > type;
};

//--- deref ----------------------------------------------------------------------------------------
template< typename t, typename i >
	struct deref< ooe::tuple_iterator< t, i > >
	: public at< t, i >
{
};

//--- advance --------------------------------------------------------------------------------------
template< typename t, typename i, typename x >
	struct advance< ooe::tuple_iterator< t, i >, x >
{
	typedef typename ooe::tuple_iterator< t, typename plus< i, x >::type >::type type;
};

//--- distance -------------------------------------------------------------------------------------
template< typename t, typename i, typename j >
	struct distance< ooe::tuple_iterator< t, i >, ooe::tuple_iterator< t, j > >
	: public minus< j, i >
{
};

//--- at_impl --------------------------------------------------------------------------------------
template<>
	struct at_impl< ooe::tuple_tag >
{
	template< typename t, typename i >
		struct apply
		: public ooe::tuple_element< i::value, t >
	{
	};
};

//--- begin_impl -----------------------------------------------------------------------------------
template<>
	struct begin_impl< ooe::tuple_tag >
{
	template< typename t >
		struct apply
	{
		typedef ooe::tuple_iterator< t, int_< 0 > > type;
	};
};

//--- end_impl -------------------------------------------------------------------------------------
template<>
	struct end_impl< ooe::tuple_tag >
{
	template< typename t >
		struct apply
	{
		typedef ooe::tuple_iterator< t, int_< ooe::tuple_size< t >::value > > type;
	};
};

//--- size_impl ------------------------------------------------------------------------------------
template<>
	struct size_impl< ooe::tuple_tag >
{
	template< typename t >
		struct apply
		: public ooe::tuple_size< t >
	{
	};
};

//--- clear_impl -----------------------------------------------------------------------------------
template<>
	struct clear_impl< ooe::tuple_tag >
{
	template< typename >
		struct apply
		: public ooe::tuple<>
	{
	};
};

//--- push_front_impl ------------------------------------------------------------------------------
template<>
	struct push_front_impl< ooe::tuple_tag >
{
	template< typename t, typename x >
		struct apply
		: public ooe::tuple_push_front< x, t >
	{
	};
};

//--- push_back_impl -------------------------------------------------------------------------------
template<>
	struct push_back_impl< ooe::tuple_tag >
{
	template< typename t, typename x >
		struct apply
		: public ooe::tuple_push_back< x, t >
	{
	};
};

OOE_NAMESPACE_END( ( boost )( mpl ) )

	#endif	// OOE_FOUNDATION_UTILITY_TUPLE_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- tuple_base -----------------------------------------------------------------------------------
template< typename t >
	struct tuple_base< LIMIT, t, true >
	: private t
{
	typedef t BOOST_PP_CAT( t, LIMIT );

	tuple_base( void ) {}

	tuple_base( typename call_traits< t >::param_type a )
		: t( a )
	{
	}

	template< typename s >
		tuple_base( const s& a )
		: t( a )
	{
	}
};

template< typename t >
	struct tuple_base< LIMIT, t, false >
{
	typedef t BOOST_PP_CAT( t, LIMIT );
	t BOOST_PP_CAT( _, LIMIT );

	tuple_base( void ) {}

	tuple_base( typename call_traits< t >::param_type a )
		: BOOST_PP_CAT( _, LIMIT )( a )
	{
	}

	template< typename s >
		tuple_base( const s& a )
		: BOOST_PP_CAT( _, LIMIT )( a )
	{
	}
};

#define INHERIT( z, n, _ ) public tuple_base< n, t ## n, is_empty< t ## n >::value >
#define CONSTRUCT( z, n, d ) tuple_base< n, t ## n, is_empty< t ## n >::value >( d ## n )

//--- tuple ----------------------------------------------------------------------------------------
template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
	struct tuple
#else
	struct tuple< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >
#endif
	BOOST_PP_EXPR_IF( LIMIT, : ) BOOST_PP_ENUM( LIMIT, INHERIT, ~ )
{
	typedef tuple_tag tag;
	typedef tuple type;

	static const unsigned size = LIMIT;

	tuple( void ) {}

#if LIMIT
	tuple( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, typename call_traits< t, >::param_type a ) )
		BOOST_PP_EXPR_IF( LIMIT, : ) BOOST_PP_ENUM( LIMIT, CONSTRUCT, a )
	{
	}

	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename s ) >
		tuple( const tuple< BOOST_PP_ENUM_PARAMS( LIMIT, s ) >& a )
		BOOST_PP_EXPR_IF( LIMIT, : ) BOOST_PP_ENUM( LIMIT, CONSTRUCT, a._ )
	{
	}
#endif

#if LIMIT == 2
	template< typename s0, typename s1 >
		tuple( const std::pair< s0, s1 >& pair )
		: tuple_base< 0, t0, is_empty< t0 >::value >( pair.first ),
		tuple_base< 1, t1, is_empty< t1 >::value >( pair.second )
	{
	}

	operator std::pair< t0, t1 >( void ) const
	{
		return std::pair< t0, t1 >( this->_0, this->_1 );
	}
#endif
};

#undef CONSTRUCT
#undef INHERIT

//--- make_tuple -----------------------------------------------------------------------------------
#if LIMIT
template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#endif
	inline tuple< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >
	make_tuple( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, t, a ) )
{
	return tuple< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
}

#if LIMIT != OOE_PP_LIMIT
//--- tuple_push_front -----------------------------------------------------------------------------
template< typename x, typename t >
	struct tuple_push_front< x, t,
	typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
{
	typedef tuple< x BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t::t ) > type;
};

//--- tuple_push_back ------------------------------------------------------------------------------
template< typename x, typename t >
	struct tuple_push_back< x, t,
	typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
{
	typedef tuple< BOOST_PP_ENUM_PARAMS( LIMIT, typename t::t ) BOOST_PP_COMMA_IF( LIMIT ) x > type;
};
#endif

#if LIMIT
#define MINUS BOOST_PP_DEC( LIMIT )

//--- tuple_element --------------------------------------------------------------------------------
template< typename t >
	struct tuple_element< MINUS, t >
{
	typedef typename no_ref< t >::type:: BOOST_PP_CAT( t, MINUS ) type;
};

//--- tuple_equal ----------------------------------------------------------------------------------
template< typename a, typename b >
	struct tuple_equal< LIMIT, a, b >
{
	static bool call( const a& x, const b& y )
	{
		return tuple_equal< MINUS, a, b >::call( x, y ) &&
			at< MINUS >( x ) == at< MINUS >( y );
	}
};

//--- tuple_less -----------------------------------------------------------------------------------
template< typename a, typename b >
	struct tuple_less< LIMIT, a, b >
{
	static bool call( const a& x, const b& y )
	{
		return tuple_less< MINUS, a, b >::call( x, y ) ||
			( !( at< MINUS >( y ) < at< MINUS >( x ) ) &&
			at< LIMIT >( x ) < at< LIMIT >( y ) );
	}
};

#undef MINUS
#endif

//--- tuple_at -------------------------------------------------------------------------------------
template< typename t >
	struct tuple_at< LIMIT, t,
	typename enable_if< is_empty< typename tuple_element< LIMIT, t >::type > >::type >
{
	typedef typename tuple_element< LIMIT, t >::type t0;
	typedef typename add_const< t0 >::type t1;
	typedef typename boost::mpl::if_< is_const< t >, t1, t0 >::type t2;
	typedef typename add_reference< t2 >::type type;

	static type call( typename call_traits< t >::reference tuple )
	{
		typedef tuple_base< LIMIT, t0, true > b0;
		typedef typename add_const< b0 >::type b1;
		typedef typename boost::mpl::if_< is_const< t >, b1, b0 >::type b2;
		return static_cast< b2& >( tuple );
	}
};

template< typename t >
	struct tuple_at< LIMIT, t,
	typename disable_if< is_empty< typename tuple_element< LIMIT, t >::type > >::type >
{
	typedef typename tuple_element< LIMIT, t >::type t0;
	typedef typename add_const< t0 >::type t1;
	typedef typename boost::mpl::if_< is_const< t >, t1, t0 >::type t2;
	typedef typename add_reference< t2 >::type type;

	static type call( typename call_traits< t >::reference tuple )
	{
		return BOOST_PP_CAT( tuple._, LIMIT );
	}
};

//--- operator << ----------------------------------------------------------------------------------
template< typename type BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	type& operator <<( type& out, const tuple< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >& in )
{
	out << "( ";
	typedef tuple_size< tuple< BOOST_PP_ENUM_PARAMS( LIMIT, t ) > > size_type;
	tuple_print< size_type::value >::call( out, in );
	return out << " )";
}

OOE_NAMESPACE_END( ( ooe ) )

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
