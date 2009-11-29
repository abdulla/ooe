/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/javascript/facade.hpp"
#include "component/registry/local.hpp"
#include "component/registry/registry.hpp"

namespace
{
	using namespace ooe;

	class source
	{
	public:
		source( const std::string& path )
			: library( path ), module( library.find< ooe::module ( void ) >( "module_open" )() )
		{
		}

		const ooe::module& get( void ) const
		{
			return module;
		}

	private:
		ooe::library library;
		ooe::module module;
	};

	template< v8::Handle< v8::Value > ( * function )( const v8::Arguments& arguments ) >
		struct embed
	{
		struct defer
		{
			static v8::Handle< v8::Value > call( const v8::Arguments& arguments )
			{
				return function( arguments );
			}
		};

		static v8::Handle< v8::Value > call( const v8::Arguments& arguments )
		{
			return javascript::invoke< defer >::call( arguments );
		}
	};

	typedef std::vector< std::string > find_vector;

	v8::Handle< v8::Value > find( const v8::Arguments& arguments )
	{
		javascript::verify_arguments( arguments, 1 );
		v8::HandleScope scope;

		find_vector input;
		javascript::to< find_vector >::call( arguments[ 0 ], input );
		interface interface;

		for ( find_vector::const_iterator i = input.begin(), end = input.end(); i != end; ++i )
		{
			up_t j = i->find( '/' );

			if ( j == up_t( -1 ) )
				throw error::runtime( "javascript::find: " ) << "Invalid function \"" << *i << '\"';

			interface.insert( i->substr( 0, j ), i->substr( j + 1 ) );
		}

		registry registry;
		registry::info_vector v = registry.find( interface );
		find_vector output;

		for ( registry::info_vector::const_iterator i = v.begin(), end = v.end(); i != end; ++i )
		{
			if ( i->_0 == registry::library )
				output.push_back( i->_1 );
		}

		return javascript::from< find_vector >::call( output );
	}

	v8::Handle< v8::Value > load( const v8::Arguments& arguments )
	{
		javascript::verify_arguments( arguments, 1 );
		v8::HandleScope scope;

		std::string path;
		javascript::to< std::string >::call( arguments[ 0 ], path );

		shared_ptr< source > source_ptr = new source( path );
		const module& module = source_ptr->get();
		const module::vector_type& names = module.get();
		const facade::local::vector_type& local = static_cast< const facade::local* >
			( module.find( typeid( facade::local ).name() ) )->get();
		const facade::javascript::vector_type& javascript = static_cast< const facade::javascript* >
			( module.find( typeid( facade::javascript ).name() ) )->get();

		v8::Handle< v8::Object > object = v8::Object::New();

		for ( up_t i = 0, end = names.size(); i != end; ++i )
		{
			v8::Handle< v8::Value > key =
				javascript::from< std::string >::call( names[ i ]._0 + '/' + names[ i ]._1 );
			v8::Handle< v8::Value > data;

			if ( names[ i ]._1[ 0 ] == 'F' )
				data = javascript::from< void ( * )( void ) >::call( local[ i ].function );
			else if ( names[ i ]._1[ 0 ] == 'M' )
				data = javascript::from< void ( any::* )( void ) >::call( local[ i ].member );
			else
				throw error::runtime( "javascript::load: " ) << "Function \"" << names[ i ]._0 <<
					"\" of type \"" << names[ i ]._1 << "\" has unknown pointer";

			object->Set( key,
				v8::FunctionTemplate::New( javascript[ i ], data )->GetFunction() );
		}

		object->Set( javascript::from< up_t >::call( 0 ),
			javascript::from< shared_ptr< source > >::call( source_ptr ) );
		return object;
	}

	v8::Handle< v8::Value > print( const v8::Arguments& arguments )
	{
		v8::HandleScope scope;

		for ( up_t i = 0, end = arguments.Length(); i != end; ++i )
		{
			if ( i )
				std::cout << ' ';

			v8::String::Utf8Value string( arguments[ i ] );
			std::cout.write( *string, string.length() );
		}

		std::cout << '\n';
		return v8::Undefined();
	}
}

namespace ooe
{
//--- facade::javascript -------------------------------------------------------
	const facade::javascript::vector_type& facade::javascript::get( void ) const
	{
		return vector;
	}

	void facade::javascript::insert( up_t index, v8::InvocationCallback call )
	{
		vector_type::iterator i = vector.begin();
		std::advance( i, index );
		vector.insert( i, call );
	}

//--- javascript ---------------------------------------------------------------
	void javascript::component_setup( v8::Handle< v8::Object > global, const std::string& path )
	{
		v8::Handle< v8::Object > ooe = v8::Object::New();

		//--- executable -------------------------------------------------------
		v8::Handle< v8::Object > executable = v8::Object::New();
		ooe->Set( from< const c8* >::call( "executable" ), executable );

		executable->Set( from< const c8* >::call( "path" ), from< std::string >::call( path ) );

		//--- library ----------------------------------------------------------
		v8::Handle< v8::Object > library = v8::Object::New();
		ooe->Set( from< const c8* >::call( "library" ), library );

		library->Set( from< const c8* >::call( "suffix" ),
			from< const c8* >::call( library::suffix ) );

		//--- registry ---------------------------------------------------------
		v8::Handle< v8::Object > registry = v8::Object::New();
		ooe->Set( from< const c8* >::call( "registry" ), registry );

		registry->Set( from< const c8* >::call( "find" ),
			v8::FunctionTemplate::New( embed< find >::call )->GetFunction() );
		registry->Set( from< const c8* >::call( "load" ),
			v8::FunctionTemplate::New( embed< load >::call )->GetFunction() );

		//----------------------------------------------------------------------

		global->Set( from< const c8* >::call( "print" ),
			v8::FunctionTemplate::New( print )->GetFunction() );

		global->Set( from< const c8* >::call( "ooe" ), ooe );
	}
}
