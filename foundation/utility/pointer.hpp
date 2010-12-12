/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_POINTER_HPP
#define OOE_FOUNDATION_UTILITY_POINTER_HPP

#include "foundation/utility/noncopyable.hpp"

#ifdef __linux__
    #ifndef __THROW
    #define __THROW throw()
    #endif
#else
    #define __THROW
#endif

extern "C" void free( void* ) __THROW;

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- deallocate_ptr -------------------------------------------------------------------------------
template< typename type >
    void deallocate_ptr( type* value )
{
    delete value;
}

//--- deallocate_array -----------------------------------------------------------------------------
template< typename type >
    void deallocate_array( type* value )
{
    delete[] value;
}

//--- deallocate_free ------------------------------------------------------------------------------
template< typename type >
    void deallocate_free( type* value )
{
    free( value );
}

//--- scoped_base ----------------------------------------------------------------------------------
template< typename type, void ( * function )( type* ) >
    class scoped_base
    : private noncopyable
{
public:
    operator type*( void ) const
    {
        return value;
    }

    type* operator ->( void ) const
    {
        return value;
    }

    type* get( void ) const
    {
        return value;
    }

    template< typename to >
        to* as( void ) const
    {
        return static_cast< to* >( value );
    }

    type* release( void )
    {
        type* pass_back = value;
        value = 0;
        return pass_back;
    }

    void swap( scoped_base& exchange )
    {
        type* save = value;
        value = exchange.value;
        exchange.value = save;
    }

protected:
    type* value;

    scoped_base( type* value_ )
        : value( value_ )
    {
    }

    ~scoped_base( void )
    {
        function( value );
    }
};

//--- scoped_dereference ---------------------------------------------------------------------------
template< typename type, void ( * function )( type* ) >
    class scoped_dereference
    : public scoped_base< type, function >
{
public:
    type& operator *( void ) const
    {
        return *this->value;
    }

protected:
    scoped_dereference( type* value_ )
        : scoped_base< type, function >( value_ )
    {
    }
};

template< void ( * function )( void* ) >
    class scoped_dereference< void, function >
    : public scoped_base< void, function >
{
protected:
    scoped_dereference( void* value_ )
        : scoped_base< void, function >( value_ )
    {
    }
};

//--- scoped_ptr -----------------------------------------------------------------------------------
template< typename type >
    struct scoped_ptr
    : public scoped_dereference< type, deallocate_ptr< type > >
{
    scoped_ptr( type* value_ )
        : scoped_dereference< type, deallocate_ptr< type > >( value_ )
    {
    }
};

//--- scoped_array ---------------------------------------------------------------------------------
template< typename type >
    struct scoped_array
    : public scoped_dereference< type, deallocate_array< type > >
{
    scoped_array( type* value_ )
        : scoped_dereference< type, deallocate_array< type > >( value_ )
    {
    }
};

//--- scoped_free ----------------------------------------------------------------------------------
template< typename type >
    struct scoped_free
    : public scoped_dereference< type, deallocate_free< type > >
{
    scoped_free( type* value_ )
        : scoped_dereference< type, deallocate_free< type > >( value_ )
    {
    }
};

//--- shared_data ----------------------------------------------------------------------------------
template< typename type, void ( * function )( type* ), typename use_t >
    class shared_data
{
public:
    shared_data( type* value_ )
        : use_count( 1 ), value( value_ )
    {
    }

    void increment( void )
    {
        ++use_count;
    }

    void decrement( void )
    {
        if ( --use_count )
            return;

        function( value );
        delete this;
    }

    bool unique( void ) const
    {
        return use_count == 1;
    }

    type* get( void ) const
    {
        return value;
    }

private:
    use_t use_count;
    type* const value;
};

//--- shared_base ----------------------------------------------------------------------------------
template< typename type, void ( * function )( type* ), typename use_t >
    class shared_base
{
public:
    shared_base( const shared_base& copy )
        : data( copy.data )
    {
        data->increment();
    }

    shared_base& operator =( const shared_base& copy )
    {
        copy.data->increment();
        data->decrement();
        data = copy.data;
        return *this;
    }

    operator type*( void ) const
    {
        return data->get();
    }

    type* operator ->( void ) const
    {
        return data->get();
    }

    bool unique( void ) const
    {
        return data->unique();
    }

    type* get( void ) const
    {
        return data->get();
    }

    template< typename to >
        to* as( void ) const
    {
        return reinterpret_cast< to* >( data->get() );
    }

    void swap( shared_base& exchange )
    {
        data_type* save = data;
        data = exchange.data;
        exchange.data = save;
    }

protected:
    typedef shared_data< type, function, use_t > data_type;
    data_type* data;

    shared_base( type* value )
        : data( new data_type( value ) )
    {
    }

    ~shared_base( void )
    {
        data->decrement();
    }
};

//--- shared_dereference ---------------------------------------------------------------------------
template< typename type, void ( * function )( type* ), typename use_t >
    class shared_dereference
    : public shared_base< type, function, use_t >
{
public:
    type& operator *( void ) const
    {
        return *this->data->get();
    }

protected:
    shared_dereference( type* value )
        : shared_base< type, function, use_t >( value )
    {
    }
};

template< void ( * function )( void* ), typename use_t >
    class shared_dereference< void, function, use_t >
    : public shared_base< void, function, use_t >
{
protected:
    shared_dereference( void* value )
        : shared_base< void, function, use_t >( value )
    {
    }
};

//--- shared_ptr -----------------------------------------------------------------------------------
template< typename type >
    struct shared_ptr
    : public shared_dereference< type, deallocate_ptr< type >, unsigned >
{
    shared_ptr( type* value = 0 )
        : shared_dereference< type, deallocate_ptr< type >, unsigned >( value )
    {
    }
};

//--- shared_array ---------------------------------------------------------------------------------
template< typename type >
    struct shared_array
    : public shared_dereference< type, deallocate_array< type >, unsigned >
{
    shared_array( type* value = 0 )
        : shared_dereference< type, deallocate_array< type >, unsigned >( value )
    {
    }
};

//--- shared_free ----------------------------------------------------------------------------------
template< typename type >
    struct shared_free
    : public shared_dereference< type, deallocate_free< type >, unsigned >
{
    shared_free( type* value = 0 )
        : shared_dereference< type, deallocate_free< type >, unsigned >( value )
    {
    }
};

//--- opaque_data ----------------------------------------------------------------------------------
class opaque_data
{
public:
    typedef void ( * function_type )( const void* );

    opaque_data( void* value_, function_type function_ )
        : use_count( 1 ), value( value_ ), function( function_ )
    {
    }

    void increment( void )
    {
        ++use_count;
    }

    void decrement( void )
    {
        if ( --use_count )
            return;

        function( value );
        delete this;
    }

    bool unique( void ) const
    {
        return use_count == 1;
    }

    void* get( void ) const
    {
        return value;
    }

    function_type destructor( void ) const
    {
        return function;
    }

private:
    unsigned use_count;
    void* const value;
    const function_type function;
};

//--- opaque_ptr -----------------------------------------------------------------------------------
class opaque_ptr
{
public:
    typedef opaque_data::function_type function_type;

    template< typename t >
        opaque_ptr( t* value )
        : data( new opaque_data( value, opaque_delete< t > ) )
    {
    }

    opaque_ptr( const opaque_ptr& copy )
        : data( copy.data )
    {
        data->increment();
    }

    ~opaque_ptr( void )
    {
        data->decrement();
    }

    opaque_ptr& operator =( const opaque_ptr& copy )
    {
        copy.data->increment();
        data->decrement();
        data = copy.data;
        return *this;
    }

    operator void*( void ) const
    {
        return data->get();
    }

    bool unique( void ) const
    {
        return data->unique();
    }

    void* get( void ) const
    {
        return data->get();
    }

    template< typename to >
        to* as( void ) const
    {
        return reinterpret_cast< to* >( data->get() );
    }

    function_type destructor( void )
    {
        return data->destructor();
    }

    void swap( opaque_ptr& exchange )
    {
        opaque_data* save = data;
        data = exchange.data;
        exchange.data = save;
    }

private:
    opaque_data* data;

    template< typename t >
        friend void opaque_delete( const void* value )
    {
        delete static_cast< const t* >( value );
    }
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_UTILITY_POINTER_HPP
