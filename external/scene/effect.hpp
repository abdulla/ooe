/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_SCENE_EFFECT_HPP
#define OOE_EXTERNAL_SCENE_EFFECT_HPP

#include <vector>

#include "foundation/general/video.hpp"
#include "foundation/io/vfs.hpp"

namespace ooe
{
//--- effect -------------------------------------------------------------------
	class OOE_VISIBLE effect
	{
	public:
		effect( const descriptor&, const vfs&, const video& );

		ooe::program& get( void ) const;

	private:
		program_ptr program;
	};

//--- variable -----------------------------------------------------------------
	class OOE_VISIBLE variable
	{
	public:
		void insert( const uniform_ptr&, const void* );
		void apply( void ) const;

	private:
		typedef tuple< uniform_ptr, const void* > tuple_type;
		typedef std::vector< tuple_type > vector_type;

		vector_type vector;		// share vector between variables somehow?
	};
}

#endif	// OOE_EXTERNAL_SCENE_EFFECT_HPP
