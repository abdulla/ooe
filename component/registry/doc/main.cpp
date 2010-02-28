/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/registry/module.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef interface::vector_type::const_iterator iterator_type;
typedef function< std::string ( const std::string&, const std::string& ) > function_type;

//--- module_doc -----------------------------------------------------------------------------------
std::string module_doc( const module& module, const std::string& name, const std::string& type )
{
	const c8* doc = module.doc( name, type );
	return doc ? doc : std::string();
}

//--- print_docs -----------------------------------------------------------------------------------
void print_docs( const interface::vector_type names, function_type doc )
{
	for ( iterator_type first = names.begin(), i = first, end = names.end(); i != end; ++i )
	{
		if ( i != first )
			std::cout << '\n';

		std::cout <<
			"Name: " << i->_0 << "\n"
			"Type: " << i->_1 << "\n"
			"      " << demangle( i->_1 ) << '\n';

		std::string documentation = doc( i->_0, i->_1 );

		if ( documentation.empty() )
			continue;

		std::cout << "\n"
			"      " << documentation << '\n';
	}
}

//--- load_library ---------------------------------------------------------------------------------
void load_library( const std::string& path )
{
	library library( path );
	ooe::module module = library.find< ooe::module ( void ) >( "module_open" )();
	interface::vector_type names = module.names();
	print_docs( names, function_type( module, module_doc ) );
}

//--- load_server ----------------------------------------------------------------------------------
void load_server( const std::string& path )
{
	ipc::memory::client client( path );
	interface::vector_type names = ipc::memory::list( client )();
	ipc::memory::doc doc( client );
	print_docs( names, function_type( doc, &ipc::memory::doc::operator () ) );
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
	for ( s32 option; ( option = getopt( argc, argv, "l:s:" ) ) != -1; )
	{
		switch ( option )
		{
		case 'l':
			load_library( optarg );
			return true;

		case 's':
			load_server( optarg );
			return true;

		default:
			break;
		}
	}

	std::cout <<
		"    -l <path>  Path of library module to load for documentation\n"
		"    -s <path>  Path of server module to load for documentation\n";

	return false;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
