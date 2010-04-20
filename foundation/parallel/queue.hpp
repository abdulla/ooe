/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_PARALLEL_QUEUE_HPP
#define OOE_FOUNDATION_PARALLEL_QUEUE_HPP

#include "foundation/utility/atom.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- queue_node_base ------------------------------------------------------------------------------
struct queue_node_base
{
	atom< queue_node_base* > next;

	queue_node_base( queue_node_base* next_ )
		: next( next_ )
	{
	}
};

//--- queue_node -----------------------------------------------------------------------------------
template< typename t >
	struct queue_node
	: public queue_node_base
{
	t value;

	queue_node( const t& value_ )
		: queue_node_base( 0 ), value( value_ )
	{
	}
};

//--- queue_base -----------------------------------------------------------------------------------
class queue_base
{
protected:
	typedef queue_node_base base_type;

	atom< base_type* > head;
	atom< base_type* > tail;

	queue_base( base_type* node )
		: head( node ), tail( node )
	{
	}

	~queue_base( void )
	{
	}
};

//--- queue ----------------------------------------------------------------------------------------
template< typename t >
	struct queue
	: private queue_base
{
	typedef queue_node< t > node_type;

	queue( void )
		: queue_base( new node_type( t() ) )
	{
	}

	~queue( void )
	{
		for ( base_type* i = head; i; )
		{
			base_type* next = i->next;
			delete static_cast< node_type* >( i );
			i = next;
		}
	}

	// implements Michael and Scott's algorithm from:
	// http://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html
	void enqueue( const t& value )
	{
		base_type* node = new node_type( value );

		while ( true )
		{
			base_type* tail_ptr = tail;
			base_type* next_ptr = tail_ptr->next;

			if ( tail_ptr != tail )
				continue;

			if ( next_ptr )
			{
				tail.cas( tail_ptr, next_ptr );
				continue;
			}

			if ( tail_ptr->next.cas( next_ptr, node ) )
				break;
		}
	}

	bool dequeue( t& value )
	{
		base_type* head_ptr;

		while ( true )
		{
			head_ptr = head;
			base_type* tail_ptr = tail;
			base_type* next_ptr = head_ptr->next;

			if ( head_ptr != head )
				continue;

			if ( head_ptr == tail_ptr )
			{
				if ( !next_ptr )
					return false;

				tail.cas( tail_ptr, next_ptr );
				continue;
			}

			value = static_cast< node_type* >( next_ptr )->value;

			if ( head.cas( head_ptr, next_ptr ) )
				break;
		}

		delete static_cast< node_type* >( head_ptr );
		return true;
	}
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_PARALLEL_QUEUE_HPP
