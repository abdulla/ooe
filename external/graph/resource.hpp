/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_GRAPH_RESOURCE_HPP
#define OOE_EXTERNAL_GRAPH_RESOURCE_HPP

#include <map>

#include "foundation/image/image.hpp"
#include "foundation/io/vfs.hpp"

#include "external/graph/scene.hpp"

namespace ooe
{
	template< typename >
		class resource;

	template< typename >
		class cache_DEPRECATED;

	typedef image ( * image_decode_t )( const descriptor& );

//--- resource< image_decode_t > -----------------------------------------------
	template<>
		class OOE_VISIBLE resource< image_decode_t >
	{
	public:
		void insert( const std::string&, image_decode_t );
		image_decode_t operator []( const std::string& u ) const;

	private:
		typedef std::map< const std::string, image_decode_t > map_type;

		map_type map;
	};

//--- cache_DEPRECATED ---------------------------------------------------------
	template< typename type >
		class cache_DEPRECATED
	{
	public:
		cache_DEPRECATED( const ooe::vfs& vfs_, const ooe::video& video_ )
			: vfs( vfs_ ), video( video_ ), map()
		{
		}

		type& operator []( const std::string& name )
		{
			typename map_type::iterator i = map.find( name );

			if ( i != map.end() )
				return i->second;

			typename map_type::value_type value( name, type( vfs[ name ], video ) );
			return map.insert( value ).first->second;
		}

		void clear( void )
		{
			map.clear();
		}

	private:
		typedef std::map< const std::string, type > map_type;

		const ooe::vfs& vfs;
		const ooe::video& video;
		map_type map;
	};

//--- cache_DEPRECATED< texture > ----------------------------------------------
	template<>
		class OOE_VISIBLE cache_DEPRECATED< texture >
	{
	public:
		cache_DEPRECATED( const ooe::vfs&, const ooe::video&,
			const ooe::resource< image_decode_t >& );

		texture& operator []( const std::string& );
		void clear( void );

	private:
		typedef std::map< const std::string, texture_ptr > map_type;

		const ooe::vfs& vfs;
		const ooe::video& video;
		const ooe::resource< image_decode_t >& resource;
		map_type map;
	};
}

#endif	// OOE_EXTERNAL_GRAPH_RESOURCE_HPP
