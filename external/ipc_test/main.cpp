/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <iterator>
#include <set>

#include <paths.h>

#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/ipc/allocator.hpp"
#include "foundation/ipc/jumbo.hpp"
#include "foundation/ipc/string.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/nameservice.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/socket/nameservice.hpp"
#include "foundation/ipc/socket/rpc.hpp"
#include "foundation/ipc/socket/server.hpp"
#include "foundation/parallel/task.hpp"

namespace
{
	using namespace ooe;
	const up_t iteration_limit = 10000;
	const f32 microsecond_multiply = 1000000. / iteration_limit;

//--- utility ------------------------------------------------------------------
	struct print
	{
		print( void )
		{
			std::cout << "print::print( " << this << " )\n";
		}

		~print( void )
		{
			std::cout << "print::~print( " << this << " )\n";
		}

		void printer( void )
		{
			std::cout << "print::printer( " << this << " )\n";
		}

		u32 multiply( u32 i, u32 j )
		{
			return i * j;
		}
	};

	ipc::construct_ptr< print > print_construct( void )
	{
		return new print;
	}

	void print_destruct( ipc::destruct_ptr< print > p )
	{
		delete p;
	}

	typedef tuple< const c8*, const c8* > tuple_type;
	typedef std::vector< std::string > vector_type;
	typedef ipc::vector< c8 > ipc_vector;
	typedef ipc::jumbo< const c8* > jumbo_type;

	u32 test( u32 i, u32 j, const tuple_type& t, const vector_type& v )
	{
		std::cout << "test - " << i << " " << j << '\n';
		std::cout << "test - " << t << '\n';
		std::cout << "test - ";
		std::copy( v.begin(), v.end(), std::ostream_iterator< std::string >( std::cout, " " ) );
		std::cout << '\n';
		return i * j;
	}

	void vector_test( const ipc_vector& v )
	{
		std::cout << "vector_test - ";
		typedef std::ostream_iterator< ipc_vector::value_type > iterator;
		std::copy( v.begin(), v.end(), iterator( std::cout, "" ) );
		std::cout << '\n';
	}

	void jumbo_test( const jumbo_type& j )
	{
		std::cout << "jumbo_test - " << j << '\n';
	}

	void inline_test( const vector_type& v )
	{
		std::cout << "inline_test - ";
		std::copy( v.begin(), v.end(), std::ostream_iterator< std::string >( std::cout ) );
		std::cout << '\n';
	}

	void OOE_NORETURN unknown( void )
	{
		throw 0;
	}

	void null( void )
	{
	}

	void null_inline( const ipc::string& )
	{
	}

	void null_allocator( const ipc_vector& )
	{
	}

	void null_jumbo( const jumbo_type& )
	{
	}

//--- test ---------------------------------------------------------------------
	void client_test( void )
	{
		ipc::memory::client client( ipc::memory::create_semaphore, "/ooe" );
		ipc::memory::list list( client );
		ipc::memory::list::result_type result_1 = list();

		typedef ipc::memory::list::result_type::const_iterator list_iterator;

		for ( list_iterator i = result_1.begin(), end = result_1.end(); i != end; ++i )
			std::cout << "list = " << i->_0 << " " << i->_1 << '\n';

		//----------------------------------------------------------------------
		ipc::memory::find find( client );
		u32 value = find( "test", typeid( test ).name() );
		std::cout << "find = " << value << '\n';

		ipc::memory::rpc< u32 ( u32, u32, const tuple_type&, const vector_type& ) >
			test( client, value );
		value = test( 5, 7, tuple_type( "hello", "world" ), vector_type( 3, "string" ) );
		std::cout << "test = " << value << '\n';

		//----------------------------------------------------------------------
		ipc::memory::find_all find_all( client );
		ipc::memory::find_all::tuple_type tuple( "null", typeid( null ).name() );
		ipc::memory::find_all::parameter_type parameter( 3, tuple );
		ipc::memory::find_all::result_type result_2 = find_all( parameter );

		typedef ipc::memory::find_all::result_type::const_iterator find_iterator;

		for ( find_iterator i = result_2.begin(), end = result_2.end(); i != end; ++i )
			std::cout << "find_all = " << *i << '\n';

		//----------------------------------------------------------------------
		{
			ipc_vector vector;
			vector.insert( vector.end(), 64, '.' );
			std::cout << "vector name = " << vector.get_allocator().name() << '\n';

			ipc::memory::call< void ( const ipc_vector& ) > vector_test( client, "vector_test" );
			vector_test( vector );
		}

		//----------------------------------------------------------------------
		{
			jumbo_type jumbo;
			jumbo( "a brimful of asha" );
			std::cout << "jumbo name = " << jumbo.name() << '\n';

			ipc::memory::call< void ( const jumbo_type& ) > jumbo_test( client, "jumbo_test" );
			jumbo_test( jumbo );
		}

		//----------------------------------------------------------------------
		ipc::memory::call< void ( const vector_type& ) > inline_test( client, "inline_test" );
		inline_test( vector_type( 10 * 1024, "." ) );

		//----------------------------------------------------------------------
		ipc::memory::call< ipc::construct_ptr< print > ( void ) >
			print_construct( client, "print_construct" );
		print* p = print_construct();

		ipc::memory::call< void ( print* ) > printer( client, "printer" );
		printer( p );

		ipc::memory::call< u32 ( print*, u32, u32 ) > multiply( client, "multiply" );
		value = multiply( p, 2, 8 );
		std::cout << "multiply( " << p << ", 2, 8 ) = " << value << '\n';

		ipc::memory::call< void ( ipc::destruct_ptr< print > ) >
			print_destruct( client, "print_destruct" );
		print_destruct( p );

		//----------------------------------------------------------------------
		std::cout << "leaking = " << print_construct() << '\n';	// leak

		//----------------------------------------------------------------------
		ipc::memory::call< void ( void ) > null( client, "null" );
		timer timer;

		for ( up_t i = 0; i != iteration_limit; ++i )
			null();

		f32 elapsed = timer.elapsed();
		std::cout << "elapsed time = " << elapsed << " seconds\n";
	}

	void client_fail( void )
	{
		ipc::memory::client client( ipc::memory::create_semaphore, "/ooe" );

		ipc::memory::rpc< void ( void ) > out_of_range( client, static_cast< u32 >( -1 ) );
		OOE_PRINT( "CLIENT_FAIL", out_of_range() );	// throw!

		ipc::memory::find_all find_all( client );
		ipc::memory::find_all::tuple_type tuple( "no_function", "no_type" );
		ipc::memory::find_all::parameter_type parameter( 3, tuple );
		OOE_PRINT( "\nCLIENT_FAIL", find_all( parameter ) );

		ipc::memory::call< void ( print* ) > printer( client, "printer" );
		OOE_PRINT( "\nCLIENT_FAIL", printer( 0 ) );

		ipc::memory::call< void ( void ) > unknown( client, "unknown" );
		OOE_PRINT( "\nCLIENT_FAIL", unknown() );
	}

	void client_ptr( void )
	{
		ipc::memory::client client( ipc::memory::create_semaphore, "/ooe" );
		ipc::memory::call< ipc::construct_ptr< print > ( void ) >
			print_construct( client, "print_construct" );
		ipc::memory::call< void ( print* ) > printer( client, "printer" );
		std::set< print* > set;

		for ( up_t i = 0; i != iteration_limit; ++i )
			set.insert( print_construct() );

		timer timer;
		printer( *set.begin() );
		f32 elapsed = timer.elapsed();
		std::cout << "elapsed time = " << elapsed * 1000000 << " microseconds\n";
	}

	void server_test( void )
	{
		ipc::memory::nameservice nameservice;
		nameservice.insert( "test", test );
		nameservice.insert( "vector_test", vector_test );
		nameservice.insert( "jumbo_test", jumbo_test );
		nameservice.insert( "inline_test", inline_test );
		nameservice.insert( "print_construct", print_construct );
		nameservice.insert( "print_destruct", print_destruct );
		nameservice.insert( "printer", &print::printer );
		nameservice.insert( "multiply", &print::multiply );
		nameservice.insert( "null", null );
		nameservice.insert( "unknown", unknown );

		ipc::memory::server server( ipc::memory::create_semaphore, "/ooe", nameservice );

		while ( !executable::signal() )
			server.decode();
	}

	void client_ntest( void )
	{
		ipc::socket::client client( local( _PATH_TMP "ooe" ) );

		ipc::socket::rpc< void ( void ) > out_of_range( client, static_cast< u32 >( -1 ) );
		OOE_PRINT( "CLIENT_FAIL", out_of_range()() );	// throw!

		ipc::socket::call< void ( print* ) > printer( client, "printer" );
		OOE_PRINT( "\nCLIENT_FAIL", printer( 0 )() );

		ipc::socket::call< void ( void ) > unknown( client, "unknown" );
		OOE_PRINT( "\nCLIENT_FAIL", unknown()() );

		ipc::socket::call< ipc::construct_ptr< print > ( void ) >
			print_construct( client, "print_construct" );

		for ( up_t i = 0; i != iteration_limit; ++i )
			print_construct();
	}

	void server_ntest( void )
	{
		ipc::socket::nameservice nameservice;
		nameservice.insert( "print_construct", print_construct );
		nameservice.insert( "print_destruct", print_destruct );
		nameservice.insert( "printer", &print::printer );
		nameservice.insert( "unknown", unknown );

		unlink( _PATH_TMP "ooe" );
		ipc::socket::server server( local( _PATH_TMP "ooe" ) );

		while ( !executable::signal() )
			server.accept( nameservice );
	}

//--- run ----------------------------------------------------------------------
	typedef ipc::memory::rpc< void ( void ) > rpc_type;
	typedef ipc::socket::rpc< void ( void ) > nrpc_type;

	f32 run_ipc( const rpc_type& rpc, up_t limit )
	{
		timer timer;

		for ( up_t i = 0; i != limit; ++i )
			rpc();

		return timer.elapsed();
	}

	f32 run_nipc( nrpc_type& rpc, up_t limit )
	{
		timer timer;

		for ( up_t i = 1; i != limit; ++i )
			rpc();

		rpc()();
		return timer.elapsed();
	}

	f32 run_task_ipc( ipc::memory::client& client, u32 value )
	{
		rpc_type null( client, value );
		return run_ipc( null, iteration_limit );
	}

	f32 run_task_nipc( ipc::socket::client& client, u32 value )
	{
		nrpc_type null( client, value );
		return run_nipc( null, iteration_limit );
	}

	void run_load( void )
	{
		while ( !executable::signal() ) {}
	}

//--- client -------------------------------------------------------------------
	f32 client_ipc( ipc::memory::transport_type type )
	{
		ipc::memory::client client( type, "/ooe" );
		ipc::memory::find find( client );
		u32 value = find( "null", typeid( null ).name() );
		rpc_type null( client, value );

		return run_ipc( null, iteration_limit );
	}

	f32 client_nipc( const address& address )
	{
		ipc::socket::client client( address );
		ipc::socket::find find( client );
		u32 value = find( "null", typeid( null ).name() )();
		nrpc_type null( client, value );

		return run_nipc( null, iteration_limit );
	}

	f32 client_task_ipc( ipc::memory::transport_type type, up_t size )
	{
		typedef shared_ptr< ipc::memory::client > client_pointer;
		scoped_array< client_pointer > client_array( new client_pointer[ size ] );

		for ( up_t i = 0; i != size; ++i )
			client_array[ i ] = new ipc::memory::client( type, "/ooe" );

		ipc::memory::find find( *client_array[ 0 ] );
		u32 value = find( "null", typeid( null ).name() );

		typedef unique_task< f32 ( ipc::memory::client&, u32 ) > task_type;
		typedef shared_ptr< task_type > task_pointer;
		scoped_array< task_pointer > task_array( new task_pointer[ size ] );

		for ( up_t i = 0; i != size; ++i )
			task_array[ i ] = new task_type( run_task_ipc, *client_array[ i ], value );

		f32 elapsed = 0;

		for ( up_t i = 0; i != size; ++i )
			elapsed += ( *task_array[ i ] )();

		return elapsed / static_cast< f32 >( size );
	}

	f32 client_task_nipc( const address& address, up_t size )
	{
		typedef shared_ptr< ipc::socket::client > client_pointer;
		scoped_array< client_pointer > client_array( new client_pointer[ size ] );

		for ( up_t i = 0; i != size; ++i )
			client_array[ i ] = new ipc::socket::client( address );

		ipc::socket::find find( *client_array[ 0 ] );
		u32 value = find( "null", typeid( null ).name() )();

		typedef unique_task< f32 ( ipc::socket::client&, u32 ) > task_type;
		typedef shared_ptr< task_type > task_pointer;
		scoped_array< task_pointer > task_array( new task_pointer[ size ] );

		for ( up_t i = 0; i != size; ++i )
			task_array[ i ] = new task_type( run_task_nipc, *client_array[ i ], value );

		f32 elapsed = 0;

		for ( up_t i = 0; i != size; ++i )
			elapsed += ( *task_array[ i ] )();

		return elapsed / static_cast< f32 >( size );
	}

	f32 client_inline_ipc( ipc::memory::transport_type type, up_t size )
	{
		ipc::memory::client client( type, "/ooe" );
		ipc::memory::find find( client );
		ipc::memory::call< void ( const ipc::string& ) > rpc( client, "null_inline" );

		std::string string( size - 1, 'i' );
		timer timer;

		for ( up_t i = 0; i != iteration_limit; ++i )
			rpc( string );

		return timer.elapsed();
	}

	f32 client_inline_nipc( const address& address, up_t size )
	{
		ipc::socket::client client( address );
		ipc::socket::find find( client );
		ipc::socket::call< void ( const ipc::string& ) > rpc( client, "null_inline" );

		std::string string( size - 1, 'n' );
		timer timer;

		for ( up_t i = 1; i != iteration_limit; ++i )
			rpc( string );

		rpc( string )();
		return timer.elapsed();
	}

	f32 client_allocator( ipc::memory::transport_type type, up_t size )
	{
		ipc::memory::client client( type, "/ooe" );
		ipc::memory::find find( client );
		u32 value = find( "null_allocator", typeid( null_allocator ).name() );
		ipc::memory::rpc< void ( const ipc_vector& ) > rpc( client, value );

		ipc_vector vector;
		vector.insert( vector.end(), size - 1, 'a' );
		vector.push_back( 0 );

		timer timer;

		for ( up_t i = 0; i != iteration_limit; ++i )
			rpc( vector );

		return timer.elapsed();
	}

	f32 client_jumbo( ipc::memory::transport_type type, up_t size )
	{
		ipc::memory::client client( type, "/ooe" );
		ipc::memory::find find( client );
		u32 value = find( "null_jumbo", typeid( null_jumbo ).name() );
		ipc::memory::rpc< void ( const jumbo_type& ) > rpc( client, value );

		jumbo_type jumbo;
		jumbo( std::string( size - 1, 'j' ).c_str() );

		timer timer;

		for ( up_t i = 0; i != iteration_limit; ++i )
			rpc( jumbo );

		return timer.elapsed();
	}

//--- server -------------------------------------------------------------------
	void server_ipc( ipc::memory::transport_type type )
	{
		ipc::memory::nameservice nameservice;
		nameservice.insert( "null", null );
		nameservice.insert( "null_inline", null_inline );
		nameservice.insert( "null_allocator", null_allocator );
		nameservice.insert( "null_jumbo", null_jumbo );

		ipc::memory::server server( type, "/ooe", nameservice );

		while ( !executable::signal() )
			server.decode();
	}

	void server_nipc( const address& address )
	{
		ipc::socket::nameservice nameservice;
		nameservice.insert( "null", null );
		nameservice.insert( "null_inline", null_inline );

		ipc::socket::server server( address );

		while ( !executable::signal() )
			server.accept( nameservice );
	}

//--- launch -------------------------------------------------------------------
	bool launch_client( up_t index, up_t )
	{
		f32 elapsed;

		switch ( index )
		{
		case 0:
			elapsed = client_ipc( ipc::memory::create_spinlock );
			break;

		case 1:
			elapsed = client_ipc( ipc::memory::create_semaphore );
			break;

		case 2:
			elapsed = client_nipc( local( _PATH_TMP "ooe" ) );
			break;

		case 3:
			elapsed = client_nipc( internet( "localhost", 1111 ) );
			break;

		default:
			std::cout << "Unknown client method " << index << '\n';
			return false;
		}

		std::cout << elapsed * microsecond_multiply << '\n';
		return true;
	}

	bool launch_server( up_t index, up_t )
	{
		switch ( index )
		{
		case 0:
			server_ipc( ipc::memory::create_spinlock );
			break;

		case 4:
		case 1:
			server_ipc( ipc::memory::create_semaphore );
			break;

		case 2:
			unlink( _PATH_TMP "ooe" );
			server_nipc( local( _PATH_TMP "ooe" ) );
			break;

		case 3:
			server_nipc( internet( "localhost", 1111 ) );
			break;

		default:
			std::cout << "Unknown client method " << index << '\n';
			return false;
		}

		return true;
	}

	bool launch_load( up_t index, up_t )
	{
		if ( !index )
		{
			epoch_t sleep( 60 * 60 * 24, 0 );
			timer::sleep( sleep );
		}

		while ( --index )
			unique_task< void ( void ) > task( run_load );

		run_load();
		return true;
	}

	bool launch_task( up_t index, up_t size )
	{
		if ( !size )
		{
			std::cout << "Must specify a size greater than 0\n";
			return false;
		}

		f32 elapsed;

		switch ( index )
		{
		case 0:
			elapsed = client_task_ipc( ipc::memory::create_spinlock, size );
			break;

		case 1:
			elapsed = client_task_ipc( ipc::memory::create_semaphore, size );
			break;

		case 2:
			elapsed = client_task_nipc( local( _PATH_TMP "ooe" ), size );
			break;

		case 3:
			elapsed = client_task_nipc( internet( "localhost", 1111 ), size );
			break;

		default:
			std::cout << "Unknown client method " << index << '\n';
			return false;
		}

		std::cout << elapsed * microsecond_multiply << '\n';
		return true;
	}

	bool launch_inline( up_t index, up_t size )
	{
		if ( !size )
		{
			std::cout << "Must specify a size greater than 0\n";
			return false;
		}

		f32 elapsed;

		switch ( index )
		{
		case 0:
			elapsed = client_inline_ipc( ipc::memory::create_spinlock, size );
			break;

		case 1:
			elapsed = client_inline_ipc( ipc::memory::create_semaphore, size );
			break;

		case 2:
			elapsed = client_inline_nipc( local( _PATH_TMP "ooe" ), size );
			break;

		case 3:
			elapsed = client_inline_nipc( internet( "localhost", 1111 ), size );
			break;

		default:
			std::cout << "Unknown client method " << index << '\n';
			return false;
		}

		std::cout << elapsed * microsecond_multiply << '\n';
		return true;
	}

	bool launch_outline( up_t index, up_t size )
	{
		if ( !size )
		{
			std::cout << "Must specify a size greater than 0\n";
			return false;
		}

		f32 elapsed;

		switch ( index )
		{
		case 0:
			elapsed = client_allocator( ipc::memory::create_spinlock, size );
			break;

		case 1:
			elapsed = client_allocator( ipc::memory::create_semaphore, size );
			break;

		case 2:
			elapsed = client_jumbo( ipc::memory::create_spinlock, size );
			break;

		case 3:
			elapsed = client_jumbo( ipc::memory::create_semaphore, size );
			break;

		default:
			std::cout << "Unknown client method " << index << '\n';
			return false;
		}

		std::cout << elapsed * microsecond_multiply << '\n';
		return true;
	}

	bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
	{
		const c8* argument = 0;
		bool ( * function )( up_t, up_t ) = 0;

		for ( s32 option; ( option = getopt( argc, argv, "c:s:l:t:i:o:uvpxyzh" ) ) != -1; )
		{
			switch ( option )
			{
			case 'c':
				function = launch_client;
				argument = optarg;
				break;

			case 's':
				function = launch_server;
				argument = optarg;
				break;

			case 'l':
				function = launch_load;
				argument = optarg;
				break;

			case 't':
				function = launch_task;
				argument = optarg;
				break;

			case 'i':
				function = launch_inline;
				argument = optarg;
				break;

			case 'o':
				function = launch_outline;
				argument = optarg;
				break;

			case 'u':
				server_ntest();
				return true;

			case 'v':
				client_ntest();
				return true;

			case 'x':
				server_test();
				return true;

			case 'y':
				client_test();
				return true;

			case 'z':
				client_fail();
				return true;

			case 'p':
				client_ptr();
				return true;

			default:
				std::cout <<
					"    -c <method>            Start client\n"
					"    -s <method>            Start server\n"
					"\n"
					"    -l <threads>           Emulate CPU load\n"
					"\n"
					"    -t <method> <threads>  Start client (task)\n"
					"    -i <method> <size>     Start client (inline)\n"
					"    -o <method> <size>     Start client (outline)\n"
					"\n"
					"    -u                     Test socket-based server\n"
					"    -v                     Test socket-based client 1 (feature)\n"
					"\n"
					"    -x                     Test memory-based server\n"
					"    -y                     Test memory-based client 1 (feature)\n"
					"    -z                     Test memory-based client 2 (failure)\n"
					"    -p                     Test memory-based client 3 (pointer)\n";

				return false;
			}
		}

		if ( !function )
		{
			std::cout << "Select an action to perform\n";
			return false;
		}

		up_t index = !argument ? 0 : std::strtoul( argument, 0, 10 );
		up_t size = optind == argc ? 0 : std::strtoul( argv[ optind ], 0, 10 );
		return function( index, size );
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
