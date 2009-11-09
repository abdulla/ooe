/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_EXTERNAL_ENGINE_DYNAMIC_HPP
	#define OOE_EXTERNAL_ENGINE_DYNAMIC_HPP

#include <map>
#include <vector>

#include "foundation/executable/library.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/scoped.hpp"

namespace ooe
{
	class OOE_VISIBLE dynamic
	{
	public:
		typedef tuple< void*, void ( * )( void* ) > close_type;

		tuple< up_t /* libraries */, up_t /* destructors */ > size( void ) const;
		library& load( const std::string, library::type = library::local_lazy );
		void clear( void );

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "external/engine/dynamic.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	private:
		typedef shared_ptr< library > library_ptr;
		typedef std::map< const std::string, const library_ptr > map_type;
		typedef std::vector< close_type > vector_type;

		map_type map;
		vector_type vector;
	};
}

	#endif	// OOE_EXTERNAL_ENGINE_DYNAMIC_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

		template< typename type BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			type& load( const std::string& name, const std::string& symbol
			BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, & a ) )
		{
			library& library = load( name );

			typedef close_type ( function_type )( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, t, & a ) );
			function_type* function = library.find< function_type >( symbol );
			close_type close = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );

			scoped< void ( void* ) > scoped( close._1, close._0 );
			vector.push_back( close );
			scoped.clear();

			return *static_cast< type* >( close._0 );
		}

	#if LIMIT
		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
	#endif
			void load( const std::string& name, const std::string& symbol
			BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, & a ) )
		{
			load< void* >( name, symbol BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
		}

	#if LIMIT
		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
	#endif
			void run( const std::string& name, const std::string& symbol
			BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, & a ) )
		{
			library& library = load( name );

			typedef void ( function_type )( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, t, & a ) );
			function_type* function = library.find< function_type >( symbol );
			function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
