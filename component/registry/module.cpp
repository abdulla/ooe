/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/registry/module.hpp"

namespace ooe
{
//--- module -------------------------------------------------------------------
	module::module( type t, const std::string& s )
		: info_( t, s ), names_(), data_()
	{
	}

	const module::info_tuple& module::info( void ) const
	{
		return info_;
	}

	const module::name_vector& module::names( void ) const
	{
		return names_;
	}

	const module::datum_vector& module::data( void ) const
	{
		return data_;
	}
}
