/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/javascript/facade.hpp"
#include "component/registry/local.hpp"
#include "component/registry/registry.hpp"

namespace
{
	using namespace ooe;
	typedef std::vector< std::string > find_vector;

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

	v8::Handle< v8::Value > find( const v8::Arguments& arguments )
	{
		javascript::verify_arguments( arguments, 1 );

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

		std::string path;
		javascript::to< std::string >::call( arguments[ 0 ], path );

		typedef shared_ptr< ooe::source > source_ptr;
		source_ptr source = new ooe::source( path );
		const module& module = source->get();
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
			javascript::from< source_ptr >::call( source ) );
		return object;
	}

	v8::Handle< v8::Value > print( const v8::Arguments& arguments )
	{
		for ( up_t i = 0, end = arguments.Length(); i != end; ++i )
		{
			if ( i )
				std::cout << '\t';

			v8::String::AsciiValue ascii( arguments[ i ] );
			std::cout.write( *ascii, ascii.length() );
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
	void javascript::component_setup( v8::Handle< v8::Object > global )
	{
		v8::Handle< v8::Object > ooe = v8::Object::New();

		//--- registry ---------------------------------------------------------
		v8::Handle< v8::Object > registry = v8::Object::New();
		ooe->Set( from< const c8* >::call( "registry" ), registry );

		registry->Set( from< const c8* >::call( "find" ),
			v8::FunctionTemplate::New( embed< find >::call )->GetFunction() );
		registry->Set( from< const c8* >::call( "load" ),
			v8::FunctionTemplate::New( embed< load >::call )->GetFunction() );

		//----------------------------------------------------------------------

		global->Set( from< const c8* >::call( "print" ),
			v8::FunctionTemplate::New( embed< print >::call )->GetFunction() );

		global->Set( from< const c8* >::call( "ooe" ), ooe );
	}

	void javascript::throw_exception( const c8* what, const c8* where )
	{
		std::string string;
		string << what << "\n\nStack trace:" << where << "\n\nSource line:";

		v8::Handle< v8::String > error = v8::String::New( string.c_str(), string.size() );
		v8::ThrowException( v8::Exception::Error( error ) );
	}
}
