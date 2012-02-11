/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_FIXED_CACHE_HPP
#define OOE_FOUNDATION_UTILITY_FIXED_CACHE_HPP

#include <list>
#include <tr1/unordered_map>

#include "foundation/utility/namespace.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- fixed_cache ----------------------------------------------------------------------------------
template< typename k, typename m >
	class fixed_cache
{
public:
	typedef k key_type;
	typedef m mapped_type;
	typedef std::pair< key_type, mapped_type > value_type;
	typedef std::list< value_type > list_type;
	typedef typename list_type::const_iterator const_iterator;
	typedef std::tr1::unordered_map< key_type, const_iterator > map_type;
	typedef std::pair< const_iterator, bool > insert_type;

	fixed_cache( std::size_t max_ )
		: max( max_ ), list(), map( 0 )
	{
		// map.reserve( max );
	}

	std::size_t size( void ) const
	{
		return map.size();
	}

	const_iterator begin( void ) const
	{
		return list.begin();
	}

	const_iterator end( void ) const
	{
		return list.end();
	}

	const_iterator find( const key_type& key ) const
	{
		typename map_type::const_iterator mi = map.find( key );
		return mi == map.end() ? list.end() : mi->second;
	}

	insert_type insert( const value_type& value )
	{
		typename map_type::const_iterator mi = map.find( value.first );

		if ( mi != map.end() )
			return insert_type( mi->second, false );

		typename list_type::iterator li;

		if( size() < max )
		{
			list.push_back( value );
			li = --list.end();
		}
		else
		{
			li = list.begin();
			map.erase( li->first );
			*li = value;
			list.splice( list.end(), list, li );
		}

		map.insert( std::make_pair( value.first, li ) );
		return insert_type( li, true );
	}

private:
	std::size_t max;
	list_type list;
	map_type map;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_UTILITY_FIXED_CACHE_HPP