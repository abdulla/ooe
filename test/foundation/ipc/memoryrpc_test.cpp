/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/ipc/allocator.hpp"
#include "foundation/ipc/jumbo.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/nameservice.hpp"
#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/server.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;
	typedef std::vector< c8 > std_vector;
	typedef ipc::vector< c8 > ipc_vector;
	typedef ipc::jumbo< const c8* > jumbo_type;

//------------------------------------------------------------------------------
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

	void stdvector_test( const std_vector& svector )
	{
		if ( svector.size() != 64 )
			throw error::runtime( "stdvector_test: " ) << "Incorrect size";

		for ( std_vector::const_iterator i = svector.begin(), end = svector.end(); i != end; ++i )
		{
			if ( *i != '.' )
				throw error::runtime( "stdvector_test: " ) << "Incorrect data";
		}
	}

	void ipcvector_test( const ipc_vector& ivector )
	{
		if ( ivector.size() != 64 )
			throw error::runtime( "ipcvector_test: " ) << "Incorrect size";

		for ( ipc_vector::const_iterator i = ivector.begin(), end = ivector.end(); i != end; ++i )
		{
			if ( *i != '.' )
				throw error::runtime( "ipcvector_test: " ) << "Incorrect data";
		}
	}

	void jumbo_test( const jumbo_type& jumbo )
	{
		if ( std::strcmp( jumbo, "a brimful of asha" ) )
			throw error::runtime( "jumbo_test: " ) << "String mismatch";
	}

//------------------------------------------------------------------------------
	class setup
	{
	public:
		setup( void )
			: fork( 0 )
		{
			std::string name = ipc::unique_name();
			ipc::barrier_wait wait( name );
			fork_ptr( new scoped_fork ).swap( fork );

			if ( fork->is_child() )
			{
				OOE_IGNORE
				(
					executable::null_fd( STDOUT_FILENO );
					ipc::nameservice nameservice;
					nameservice.insert( "print_construct", print_construct );
					nameservice.insert( "print_destruct", print_destruct );
					nameservice.insert( "print_show", &print::show );
					nameservice.insert( "print_get", &print::get );
					nameservice.insert( "stdvector_test", stdvector_test );
					nameservice.insert( "ipcvector_test", ipcvector_test );
					nameservice.insert( "jumbo_test", jumbo_test );

					ipc::memory::server server( "/ooe.test.memory-rpc", nameservice );
					ipc::barrier_notify( name );

					while ( !executable::signal() )
						server.decode();
				);

				fork_io::exit( true );
			}
		}

	private:
		typedef scoped_ptr< scoped_fork > fork_ptr;

		fork_ptr fork;
	};

	class data
	{
	public:
		data( void )
			: client( "/ooe.test.memory-rpc" )
		{
		}

	protected:
		ipc::memory::client client;
	};

	typedef unit::group< setup, data, 4 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "memory rpc" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( setup& )
		{
			std::cerr << "test basic primitives\n";

			//--- list ---------------------------------------------------------
			typedef ipc::memory::list::result_type::const_iterator list_iterator;
			ipc::memory::list list( client );
			ipc::memory::list::result_type result_1 = list();

			for ( list_iterator i = result_1.begin(), end = result_1.end(); i != end; ++i )
				std::cout << "list = " << i->_0 << ' ' << i->_1 << '\n';

			//--- find ---------------------------------------------------------
			ipc::memory::find find( client );
			u32 value = find( "print_construct", typeid( print_construct ).name() );
			std::cout << "find = " << value << '\n';

			//--- find_all -----------------------------------------------------
			typedef ipc::memory::find_all::result_type::const_iterator find_iterator;
			typedef ipc::memory::find_all::tuple_type tuple;
			ipc::memory::find_all find_all( client );
			ipc::memory::find_all::parameter_type parameter;
			parameter.push_back( tuple( "print_construct", typeid( print_construct ).name() ) );
			parameter.push_back( tuple( "print_destruct", typeid( print_destruct ).name() ) );
			ipc::memory::find_all::result_type result_2 = find_all( parameter );

			for ( find_iterator i = result_2.begin(), end = result_2.end(); i != end; ++i )
				std::cout << "find_all = " << *i << '\n';

			//--- call ---------------------------------------------------------
			ipc::memory::call< construct_ptr< print > ( const std::string& ) >
				print_construct( client, "print_construct" );
			print* p = print_construct( "on an island in the sun" );

			ipc::memory::call< void ( destruct_ptr< print > ) >
				print_destruct( client, "print_destruct" );
			print_destruct( p );
		}

		template<>
		template<>
			void fixture_type::test< 1 >( setup& )
		{
			std::cerr << "test std::vector, ipc::vector, and ipc::jumbo\n";

			//--- std::vector --------------------------------------------------
			std_vector svector( 64, '.' );

			ipc::memory::call< void ( const std_vector& ) >( client, "stdvector_test" )( svector );

			//--- ipc::vector --------------------------------------------------
			ipc_vector ivector;
			ivector.insert( ivector.end(), 64, '.' );
			std::cout << "vector = " << ivector.get_allocator().name() << '\n';

			ipc::memory::call< void ( const ipc_vector& ) >( client, "ipcvector_test" )( ivector );

			//--- ipc::jumbo ---------------------------------------------------
			jumbo_type jumbo;
			jumbo( "a brimful of asha" );
			std::cout << "jumbo = " << jumbo.name() << '\n';

			ipc::memory::call< void ( const jumbo_type& ) >( client, "jumbo_test" )( jumbo );
		}

		template<>
		template<>
			void fixture_type::test< 2 >( setup& )
		{
			std::cerr << "test class member function and garbage collector\n";

			ipc::memory::call< construct_ptr< print > ( const std::string& ) >
				print_construct( client, "print_construct" );
			print* p = print_construct( "a three-fold utopian dream" );

			ipc::memory::call< void ( print* ) > print_show( client, "print_show" );
			print_show( p );

			ipc::memory::call< std::string ( print* ) > print_get( client, "print_get" );
			std::cout << "print::get = " << print_get( p ) << '\n';
		}

		template<>
		template<>
			void fixture_type::test< 3 >( setup& )
		{
			std::cerr << "test exceptions\n";

			ipc::memory::rpc< void ( void ) > out_of_range( client, ~u32( 0 ) );
			except( "out-of-range index", out_of_range() );

			ipc::memory::find find( client );
			except( "find non-existant function", find( "no_function", "no_type" ) );

			ipc::memory::find_all find_all( client );
			ipc::memory::find_all::tuple_type tuple( "no_function", "no_type" );
			ipc::memory::find_all::parameter_type parameter( 2, tuple );
			except( "find-all non-existant functions", find_all( parameter ) );

			ipc::memory::call< void ( print* ) > print_show( client, "print_show" );
			except( "null-pointer argument", print_show( 0 ) );

			ipc::memory::call< void ( destruct_ptr< print > ) >
				print_destruct( client, "print_destruct" );
			except( "unknown destruct_ptr argument", print_destruct( 0 ) );
		}
	}
}
