/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/switchboard.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc ) )

//--- ipc_null -------------------------------------------------------------------------------------
up_t ipc_null( const any&, io_buffer&, pool& )
{
    return 0;
}

//--- return_error ---------------------------------------------------------------------------------
up_t return_error( io_buffer& buffer, bool executed, const c8* what, const c8* where )
{
    up_t size = stream_size< bool, const c8*, const c8* >::call( executed, what, where );
    buffer.allocate( size );
    stream_write< bool, const c8*, const c8* >::call( buffer.get(), executed, what, where );
    return size;
}

OOE_ANONYMOUS_END( ( ooe )( ipc ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- switchboard ----------------------------------------------------------------------------------
switchboard::switchboard( void )
    : vector()
{
    insert_direct( ipc_null, 0 );
}

switchboard::size_type switchboard::execute( index_t index, io_buffer& buffer, pool& pool ) const
{
    bool executed = false;
    up_t result_size = 0;
    up_t error_size = 0;

    try
    {
        if ( index >= vector.size() )
            throw error::runtime( "ipc::switchboard: " ) <<
                "Unable to execute function, index " << index << " out of range";

        const tuple_type& tuple = vector[ index ];
        executed = true;
        result_size = tuple._0( tuple._1, buffer, pool );
    }
    catch ( error::verification& error )
    {
        error_size = return_error( buffer, false, error.what(), error.where() );
    }
    catch ( error::runtime& error )
    {
        error_size = return_error( buffer, executed, error.what(), error.where() );
    }
    catch ( std::exception& error )
    {
        error_size = return_error( buffer, executed, error.what(), "\nNo stack trace available" );
    }
    catch ( ... )
    {
        error_size = return_error( buffer, executed,
            "An unknown exception was thrown", "\nNo stack trace available" );
    }

    return size_type( result_size, error_size );
}

index_t switchboard::insert_direct( call_type call, any any )
{
    vector.push_back( tuple_type( call, any ) );
    return vector.size() - 1;
}

switchboard::tuple_type switchboard::operator []( index_t index ) const
{
    return vector[ index ];
}

OOE_NAMESPACE_END( ( ooe )( ipc ) )
