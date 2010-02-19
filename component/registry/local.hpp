/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_LOCAL_HPP
#define OOE_COMPONENT_REGISTRY_LOCAL_HPP

#include "component/registry/interface.hpp"
#include "component/registry/module.hpp"
#include "foundation/executable/library.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( facade ) )

//--- local ----------------------------------------------------------------------------------------
class local
{
public:
	typedef std::vector< any > vector_type;

	const vector_type& get( void ) const OOE_VISIBLE;
	void insert( up_t, any ) OOE_VISIBLE;

private:
	vector_type vector;
};

OOE_NAMESPACE_END( ( ooe )( facade ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- source ---------------------------------------------------------------------------------------
class OOE_VISIBLE source
{
public:
	source( const std::string& );
	const ooe::module& get( void ) const;

private:
	ooe::library library;
	ooe::module module;
};

//--- local ----------------------------------------------------------------------------------------
class local
{
public:
	local( const std::string& ) OOE_VISIBLE;
	bool supports( const interface& ) const OOE_VISIBLE;
	any find( const std::string&, const std::string& ) const OOE_VISIBLE;

	template< typename type >
		type find( const std::string& name,
		typename enable_if< is_function_pointer< type > >::type* = 0 ) const
	{
		typedef typename remove_pointer< type >::type function_type;
		any any = find( name, typeid( function_type ).name() );
		return reinterpret_cast< type >( any.function );
	}

	template< typename type >
		type find( const std::string& name,
		typename enable_if< is_member_function_pointer< type > >::type* = 0 ) const
	{
		typedef typename function_of< type >::type signature_type;
		any any = find( name, typeid( signature_type ).name() );
		return reinterpret_cast< type >( any.member );
	}

	template< typename type >
		const c8* doc( const std::string& name,
		typename enable_if< is_function_pointer< type > >::type* = 0 ) const
	{
		typedef typename remove_pointer< type >::type function_type;
		return source.get().doc( name, typeid( function_type ).name() );
	}

	template< typename type >
		const c8* doc( const std::string& name,
		typename enable_if< is_member_function_pointer< type > >::type* = 0 ) const
	{
		typedef typename function_of< type >::type signature_type;
		return source.get().doc( name, typeid( signature_type ).name() );
	}

private:
	ooe::source source;
	const facade::local::vector_type& vector;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_REGISTRY_LOCAL_HPP
