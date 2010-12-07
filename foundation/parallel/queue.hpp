/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_PARALLEL_QUEUE_HPP
#define OOE_FOUNDATION_PARALLEL_QUEUE_HPP

#include "foundation/utility/atom.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct queue_node_base;

//--- queue_ptr ------------------------------------------------------------------------------------
struct queue_ptr
{
    atom< queue_node_base* > ptr;
    atom< unsigned > count;

    queue_ptr( queue_node_base* ptr_ = 0, unsigned count_ = 0 )
        : ptr( ptr_ ), count( count_ )
    {
    }

    bool cas( const queue_ptr& compare, queue_node_base* value )
    {
        unsigned i = compare.count + 1;

        if ( !ptr.cas( compare.ptr, value ) )
            return false;

        count = i;
        return true;
    }
};

//--- queue_node_base ------------------------------------------------------------------------------
struct queue_node_base
{
    queue_ptr next;

    queue_node_base( void )
        : next()
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

    queue_ptr queue_head;
    queue_ptr queue_tail;

    queue_base( base_type* node )
        : queue_head( node ), queue_tail( node )
    {
    }

    ~queue_base( void )
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
        for ( base_type* node = queue_head.ptr; node; )
        {
            base_type* next = node->next.ptr;
            delete static_cast< node_type* >( node );
            node = next;
        }
    }

    // implements Michael and Scott's algorithm from:
    // http://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html
    void enqueue( const t& value )
    {
        base_type* node = new node_type( value );
        queue_ptr tail;

        while ( true )
        {
            tail = queue_tail;
            queue_ptr next = tail.ptr->next;

            if ( tail.ptr != queue_tail.ptr || tail.count != queue_tail.count )
                continue;

            if ( next.ptr )
            {
                queue_tail.cas( tail, next.ptr );
                continue;
            }

            if ( tail.ptr->next.cas( next, node ) )
                break;
        }

        queue_tail.cas( tail, node );
    }

    bool dequeue( t& value )
    {
        queue_ptr head;

        while ( true )
        {
            head = queue_head;
            queue_ptr tail = queue_tail;
            queue_ptr next = head.ptr->next;

            if ( head.ptr != queue_head.ptr || head.count != queue_head.count )
                continue;

            if ( head.ptr == tail.ptr )
            {
                if ( !next.ptr )
                    return false;

                queue_tail.cas( tail, next.ptr );
                continue;
            }

            value = next.ptr.as< node_type* >()->value;

            if ( queue_head.cas( head, next.ptr ) )
                break;
        }

        delete head.ptr.as< node_type* >();
        return true;
    }

private:
    typedef queue_node< t > node_type;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_PARALLEL_QUEUE_HPP
