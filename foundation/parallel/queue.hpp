/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_PARALLEL_QUEUE_HPP
#define OOE_FOUNDATION_PARALLEL_QUEUE_HPP

#include "foundation/utility/atom.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- queue_node_base ------------------------------------------------------------------------------
struct queue_node_base
{
    atom< queue_node_base* > next;

    queue_node_base( void )
        : next( 0 )
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
        : queue_node_base(), value( value_ )
    {
    }
};

//--- queue_base -----------------------------------------------------------------------------------
class queue_base
{
protected:
    typedef queue_node_base base_type;

    atom< base_type* > queue_head;
    atom< base_type* > queue_tail;

    queue_base( base_type* node )
        : queue_head( node ), queue_tail( node )
    {
    }
};

//--- queue ----------------------------------------------------------------------------------------
template< typename t >
    class queue
    : private queue_base
{
public:
    typedef t value_type;

    queue( void )
        : queue_base( new node_type( t() ) )
    {
    }

    ~queue( void )
    {
        for ( base_type* node = queue_head; node; )
        {
            base_type* next = node->next;
            delete static_cast< node_type* >( node );
            node = next;
        }
    }

    // implements Michael and Scott's algorithm from:
    // http://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html
    void enqueue( const t& value )
    {
        base_type* node = new node_type( value );
        base_type* tail;

        while ( true )
        {
            tail = queue_tail;
            base_type* next = tail->next;

            if ( tail != queue_tail )
                continue;

            if ( next )
            {
                queue_tail.cas( tail, next );
                continue;
            }

            if ( tail->next.cas( next, node ) )
                break;
        }

        queue_tail.cas( tail, node );
    }

    bool dequeue( t& value )
    {
        base_type* head;

        while ( true )
        {
            head = queue_head;
            base_type* tail = queue_tail;
            base_type* next = head->next;

            if ( head != queue_head )
                continue;

            if ( head == tail )
            {
                if ( !next )
                    return false;

                queue_tail.cas( tail, next );
                continue;
            }

            value = static_cast< node_type* >( next )->value;

            if ( queue_head.cas( head, next ) )
                break;
        }

        delete static_cast< node_type* >( head );
        return true;
    }

private:
    typedef queue_node< t > node_type;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_PARALLEL_QUEUE_HPP
