/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/container.hpp"
#include "foundation/ipc/jumbo.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/socket/rpc.hpp"
#include "foundation/ipc/socket/server.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef std::vector< c8 > std_vector;
typedef ipc::vector< c8 > ipc_vector;
typedef ipc::jumbo< const c8* > jumbo_type;

//--- print ----------------------------------------------------------------------------------------
class print
{
public:
    print( const std::string& value_ )
        : value( value_ )
    {
        std::cout << "print::print( " << this << " )\n";
    }

    ~print( void )
    {
        std::cout << "print::~print( " << this << " )\n";
    }

    void show( void )
    {
        std::cout << "print::show( " << this << " )\n";
    }

    std::string get( void ) const
    {
        return value;
    }

private:
    std::string value;
};

construct_ptr< print > print_construct( const std::string& value )
{
    return new print( value );
}

void print_destruct( destruct_ptr< print > print )
{
    delete print;
}

//--- stdvector_test -------------------------------------------------------------------------------
std_vector stdvector_test( const std_vector& svector )
{
    if ( svector.size() != executable::static_page_size )
        throw error::runtime( "stdvector_test: " ) << "Incorrect size";

    for ( std_vector::const_iterator i = svector.begin(), end = svector.end(); i != end; ++i )
    {
        if ( *i != '.' )
            throw error::runtime( "stdvector_test: " ) << "Incorrect data";
    }

    return svector;
}

//--- ipcvector_test -------------------------------------------------------------------------------
ipc_vector ipcvector_test( const ipc_vector& ivector )
{
    if ( ivector.size() != executable::static_page_size )
        throw error::runtime( "ipcvector_test: " ) << "Incorrect size";

    for ( ipc_vector::const_iterator i = ivector.begin(), end = ivector.end(); i != end; ++i )
    {
        if ( *i != '.' )
            throw error::runtime( "ipcvector_test: " ) << "Incorrect data";
    }

    return ivector;
}

//--- jumbo_test -----------------------------------------------------------------------------------
jumbo_type jumbo_test( const jumbo_type& jumbo )
{
    if ( std::strcmp( jumbo, "a brimful of asha" ) )
        throw error::runtime( "jumbo_test: " ) << "String mismatch";

    return jumbo;
}

//--- setup ----------------------------------------------------------------------------------------
class setup
{
public:
    setup( void )
        : fork( 0 )
    {
        std::string name = ipc::unique_name();
        ipc::barrier_wait wait( name );
        io_triplet io( -1, executable::null_fd(), -1 );
        fork.reset( new scoped_fork( io ) );

        if ( fork->is_child() )
        {
            OOE_PRINT( "socket rpc server", start_server( name ) );
            fork_io::exit( true );
        }
    }

private:
    typedef scoped_ptr< scoped_fork > fork_ptr;
    fork_ptr fork;

    void start_server( const std::string& name ) const
    {
        ipc::nameservice nameservice;
        nameservice.insert( "print_construct", print_construct );
        nameservice.insert( "print_destruct", print_destruct );
        nameservice.insert( "print_show", &print::show );
        nameservice.insert( "print_get", &print::get );
        nameservice.insert( "stdvector_test", stdvector_test );
        nameservice.insert( "ipcvector_test", ipcvector_test );
        nameservice.insert( "jumbo_test", jumbo_test );

        ipc::socket::server server( nameservice );
        listen listen( ipc::server_address( "ooe.test.socket-rpc" ) );
        ipc::barrier_notify( name );

        while ( !executable::has_signal() )
            server.insert( listen.accept() );
    }
};

//--- data -----------------------------------------------------------------------------------------
class data
{
public:
    data( void )
        : client( local_address( ipc::local_name( "ooe.test.socket-rpc" ) ) )
    {
    }

protected:
    ipc::socket::client client;
};

typedef unit::group< setup, data, 4 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "socket rpc" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "test basic primitives";

    //--- list -------------------------------------------------------------------------------------
    typedef ipc::socket::list::result_type::return_type::const_iterator list_iterator;
    ipc::socket::list list( client );
    ipc::socket::list::result_type result_1 = list();

    for ( list_iterator i = result_1().begin(), end = result_1().end(); i != end; ++i )
        std::cout << "list = " << *i << '\n';

    //--- find -------------------------------------------------------------------------------------
    ipc::socket::find find( client );
    u32 value = find( "print_construct", typeid( print_construct ).name() )();
    std::cout << "find = " << value << '\n';

    //--- find_all ---------------------------------------------------------------------------------
    typedef ipc::socket::find_all::result_type::return_type::const_iterator find_iterator;
    ipc::socket::find_all find_all( client );
    ipc::socket::find_all::parameter_type parameter;
    parameter.push_back( make_tuple( "print_construct", typeid( print_construct ).name() ) );
    parameter.push_back( make_tuple( "print_destruct", typeid( print_destruct ).name() ) );
    ipc::socket::find_all::result_type result_2 = find_all( parameter );

    for ( find_iterator i = result_2().begin(), end = result_2().end(); i != end; ++i )
        std::cout << "find_all = " << *i << '\n';

    //--- call -------------------------------------------------------------------------------------
    ipc::socket::call< construct_ptr< print > ( const std::string& ) >
        print_construct( client, "print_construct" );
    print* p = print_construct( "on an island in the sun" )();

    ipc::socket::call< void ( destruct_ptr< print > ) >
        print_destruct( client, "print_destruct" );
    print_destruct( p );
}

OOE_TEST( 1 )
{
    std::cerr << "test std::vector, ipc::vector, and ipc::jumbo";

    //--- std::vector ------------------------------------------------------------------------------
    std_vector svector( executable::static_page_size, '.' );

    ipc::socket::call< std_vector ( const std_vector& ) >( client, "stdvector_test" )( svector );

    //--- ipc::vector ------------------------------------------------------------------------------
    ipc_vector ivector;
    ivector.insert( ivector.end(), executable::static_page_size, '.' );
    std::cout << "vector = " << ivector.get_allocator().name() << '\n';

    ipc::socket::call< ipc_vector ( const ipc_vector& ) >( client, "ipcvector_test" )( ivector );

    //--- ipc::jumbo -------------------------------------------------------------------------------
    jumbo_type jumbo;
    jumbo( "a brimful of asha" );
    std::cout << "jumbo = " << jumbo.name() << '\n';

    // wait for result, to avoid race with jumbo being destructed before it is mapped by the server
    ipc::socket::call< jumbo_type ( const jumbo_type& ) >( client, "jumbo_test" )( jumbo )();
}

OOE_TEST( 2 )
{
    std::cerr << "test class member function and garbage collector";

    ipc::socket::call< construct_ptr< print > ( const std::string& ) >
        print_construct( client, "print_construct" );
    print* p = print_construct( "a three-fold utopian dream" )();

    ipc::socket::call< void ( print* ) > print_show( client, "print_show" );
    print_show( p );

    ipc::socket::call< std::string ( print* ) > print_get( client, "print_get" );
    std::cout << "print::get = " << print_get( p )() << '\n';
}

OOE_TEST( 3 )
{
    std::cerr << "test exceptions";

    ipc::socket::rpc< void ( void ) > out_of_range( client, ~u32( 0 ) );
    OOE_EXCEPT( error::runtime, out_of_range()() ) << "out-of-range index";

    ipc::socket::find find( client );
    OOE_EXCEPT( error::runtime, find( "no_function", "no_type" )() ) <<
        "find non-existant function";

    ipc::socket::find_all find_all( client );
    ipc::socket::find_all::parameter_type parameter( 2, make_tuple( "no_function", "no_type" ) );
    OOE_EXCEPT( error::runtime, find_all( parameter )() ) << "find-all non-existant functions";

    ipc::socket::call< void ( print* ) > print_show( client, "print_show" );
    OOE_EXCEPT( error::runtime, print_show( 0 )() ) << "null-pointer argument";

    ipc::socket::call< void ( destruct_ptr< print > ) > print_destruct( client, "print_destruct" );
    OOE_EXCEPT( error::runtime, print_destruct( 0 )() ) << "unknown destruct_ptr argument";
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
