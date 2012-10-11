/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/program.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

std::string ref_path;

sha256 read_ref( const std::string& name )
{
    if ( name.empty() || name[ 0 ] == '.' || name.find( '/' ) != std::string::npos )
        throw error::runtime( "storage: " ) << "Invalid name \"" << name << '\"';

    std::string path = canonical_path( ref_path + name );
    memory::region region( 0, sizeof( sha256 ) );
    memory memory( path, memory::read, region );
    return *memory.as< sha256 >();
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32, c8** )
{
    ref_path = root + "../cache/ref/";

    ipc::nameservice nameservice;
    nameservice.insert( "read_ref", read_ref );

    ipc::memory::server server( nameservice );
    listen listen( ipc::server_address( "ooe.storage" ) );

    while ( !executable::has_signal() )
        server.insert( listen.accept() );

    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
