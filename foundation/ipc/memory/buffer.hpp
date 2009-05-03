/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_BUFFER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_BUFFER_HPP

#include <list>

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/memory/name.hpp"
#include "foundation/ipc/memory/shared_memory.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace ipc
	{
		typedef tuple< u32 /* size */, bool /* external */, c8[ 32 ] /* name */ >::type header_type;
		typedef scoped_ptr< shared_memory > memory_type;

		class buffer_pack;
		struct buffer_unpack;
	}

//--- ipc::buffer_pack ---------------------------------------------------------
	class ipc::buffer_pack
	{
	public:
		typedef tuple< u8* /* buffer */, up_t /* offset */, bool /* external */ > allocate_tuple;

		buffer_pack( u8* buffer, up_t size, up_t reserved )
			: internal( 0, size - reserved, buffer + reserved ), list()
		{
			if ( reserved > size )
				throw error::runtime( "ipc::buffer_pack: " ) <<
					"Unable to reseve " << reserved << " bytes, " << size << " bytes available";
		}

		allocate_tuple allocate( up_t allocate_size )
		{
			//--- space available in internal buffer ---------------------------
			if ( internal._0 + allocate_size <= internal._1 )
			{
				up_t offset = internal._0;
				internal._0 += allocate_size;
				return allocate_tuple( internal._2 + offset, offset, false );
			}

			//-- space available in preallocated external buffers --------------
			up_t offset = 0;

			for ( list_type::iterator i = list.begin(), end = list.end(); i != end; ++i )
			{
				if ( i->_0 + allocate_size > i->_1 )
				{
					offset += i->_1;
					continue;
				}

				up_t node_used = i->_0;
				i->_0 += allocate_size;
				return allocate_tuple( i->_2 + node_used, offset + node_used, true );
			}

			//--- allocate new external buffer rounded to page boundary --------
			up_t pages = allocate_size / executable::static_page_size + 1;
			up_t node_size = pages * executable::static_page_size;
			list_tuple tuple( allocate_size, node_size, new u8[ node_size ] );
			list.push_back( tuple );
			return allocate_tuple( tuple._2, offset, true );
		}

		template< typename type >
			void store( header_type& header, type& memory, up_t used = 0 )
		{
			//--- get total size of buffer -------------------------------------
			up_t list_size = 0;

			for ( iterator_type i = list.begin(), end = list.end(); i != end; ++i )
				list_size += i->_1;

			header._1 = list_size;

			if ( !list_size )
				return;

			//--- copy in to shared memory -------------------------------------
			unique_name( header._2, sizeof( tuple_element< 2, header_type >::type ) );

			{
				type swap
					( new shared_memory( header._2, shared_memory::create, list_size + used ) );
				memory.swap( swap );
			}

			u8* out = memory->template as< u8 >() + used;

			for ( iterator_type i = list.begin(), end = list.end(); i != end; ++i )
			{
				u8* in = i->_2;
				std::copy( in, in + i->_0, out );
				out += i->_1;
			}
		}

	private:
		typedef tuple< up_t /* used */, up_t /* size */, u8* > internal_tuple;
		typedef tuple< up_t /* used */, up_t /* size */, shared_ptr< u8 > > list_tuple;
		typedef std::list< list_tuple > list_type;
		typedef list_type::const_iterator iterator_type;

		internal_tuple internal;
		list_type list;
	};

//--- ipc::buffer_unpack -------------------------------------------------------
	struct ipc::buffer_unpack
	{
		u8* internal;
		u8* external;

		buffer_unpack( u8* internal_, u8* external_ )
			: internal( internal_ ), external( external_ )
		{
		}

		buffer_unpack( u8* internal_, up_t reserved, header_type& header, memory_type& memory )
			: internal( internal_ + reserved ), external( 0 )
		{
			if ( !header._1 )
				return;

			{
				memory_type swap( new shared_memory( header._2 ) );
				memory.swap( swap );
			}

			external = memory->as< u8 >();
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_BUFFER_HPP
