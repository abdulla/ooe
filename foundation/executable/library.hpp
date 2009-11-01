/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_LIBRARY_HPP
#define OOE_FOUNDATION_EXECUTABLE_LIBRARY_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/noncopyable.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
//--- library ------------------------------------------------------------------
	class OOE_VISIBLE library
		: private noncopyable
	{
	public:
		enum type
		{
			local_lazy,
			local_now,
			global_lazy,
			global_now
		};

		enum find_type
		{
			all,
			next
		};

		library( const std::string&, type = local_lazy );
		~library( void );

		void* find( const std::string& ) const;
		static void* find( const std::string&, find_type );

		template< typename type >
			symbol< type > find( const std::string& name ) const
		{
			return find( name );
		}

		template< typename type >
			static symbol< type > find( const std::string& name, find_type flag )
		{
			return find( name, flag );
		}

	private:
		void* id;
	};
}

#endif	// OOE_FOUNDATION_EXECUTABLE_LIBRARY_HPP
