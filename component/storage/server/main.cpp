/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/storage/digest.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/file.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef std::vector< std::string > list_type;
std::string ref_path;

std::string build_path( const std::string& name )
{
    if ( name.empty() || name[ 0 ] == '.' || name.find( '/' ) != std::string::npos )
        throw error::runtime( "storage: " ) << "Invalid name \"" << name << '\"';

    return canonical_path( ref_path + name );
}

sha256 ref_read( const std::string& name )
{
    std::string path = build_path( name );
    sha256 hash;
    file( path ).read( &hash, sizeof( hash ) );
    return hash;
}

void ref_write( const std::string& name, sha256 hash )
{
    std::string path = build_path( name );
    descriptor desc( path, descriptor::write_new );
    file( desc ).write( &hash, sizeof( hash ) );
}

list_type ref_list( void )
{
    directory dir( ref_path );
    list_type list;
    std::copy( list.begin(), list.end(), std::back_inserter( list ) );
    return list;
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32, c8** )
{
    ref_path = root + "../cache/ref/";

    ipc::nameservice nameservice;
    nameservice.insert( "ref_read", ref_read );
    nameservice.insert( "ref_write", ref_write );
    nameservice.insert( "ref_list", ref_list );

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
