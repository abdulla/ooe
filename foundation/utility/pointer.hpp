/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_POINTER_HPP
#define OOE_FOUNDATION_UTILITY_POINTER_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/noncopyable.hpp"

#include OOE_PATH( foundation/utility, pointer_forward.hpp )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- deallocate_ptr -------------------------------------------------------------------------------
template< typename type >
    void deallocate_ptr( const void* value )
{
    delete static_cast< const type* >( value );
}

//--- deallocate_array -----------------------------------------------------------------------------
template< typename type >
    void deallocate_array( const void* value )
{
    delete[] static_cast< const type* >( value );
}

//--- deallocate_free ------------------------------------------------------------------------------
inline void deallocate_free( const void* value )
{
    free( const_cast< void* >( value ) );
}

//--- scoped_base ----------------------------------------------------------------------------------
template< typename type, void ( * function )( const void* ) >
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

    void reset( type* pointer = 0 )
    {
        function( value );
        value = pointer;
    }

protected:
    type* value;

    explicit scoped_base( type* value_ )
        : value( value_ )
    {
    }

    ~scoped_base( void )
    {
        function( value );
    }
};

//--- scoped_dereference ---------------------------------------------------------------------------
template< typename type, void ( * function )( const void* ) >
    class scoped_dereference
    : public scoped_base< type, function >
{
public:
    type& operator *( void ) const
    {
        return *this->value;
    }

protected:
    explicit scoped_dereference( type* value_ )
        : scoped_base< type, function >( value_ )
    {
    }
};

template< void ( * function )( const void* ) >
    class scoped_dereference< void, function >
    : public scoped_base< void, function >
{
protected:
    explicit scoped_dereference( void* value_ )
        : scoped_base< void, function >( value_ )
    {
    }
};

//--- scoped_ptr -----------------------------------------------------------------------------------
template< typename type >
    struct scoped_ptr
    : public scoped_dereference< type, deallocate_ptr< type > >
{
    explicit scoped_ptr( type* value_ )
        : scoped_dereference< type, deallocate_ptr< type > >( value_ )
    {
    }
};

//--- scoped_array ---------------------------------------------------------------------------------
template< typename type >
    struct scoped_array
    : public scoped_dereference< type, deallocate_array< type > >
{
    explicit scoped_array( type* value_ )
        : scoped_dereference< type, deallocate_array< type > >( value_ )
    {
    }
};

//--- scoped_free ----------------------------------------------------------------------------------
template< typename type >
    struct scoped_free
    : public scoped_dereference< type, deallocate_free >
{
    explicit scoped_free( type* value_ )
        : scoped_dereference< type, deallocate_free >( value_ )
    {
    }
};

//--- shared_data ----------------------------------------------------------------------------------
template< typename type, void ( * function )( const void* ), typename use_t >
    class shared_data
{
public:
    explicit shared_data( type* value_ )
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
template< typename type, void ( * function )( const void* ), typename use_t >
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

    void reset( type* value = 0 )
    {
        data->decrement();
        data = new data_type( value );
    }

protected:
    typedef shared_data< type, function, use_t > data_type;
    data_type* data;

    explicit shared_base( type* value )
        : data( new data_type( value ) )
    {
    }

    ~shared_base( void )
    {
        data->decrement();
    }
};

//--- shared_dereference ---------------------------------------------------------------------------
template< typename type, void ( * function )( const void* ), typename use_t >
    class shared_dereference
    : public shared_base< type, function, use_t >
{
public:
    type& operator *( void ) const
    {
        return *this->data->get();
    }

protected:
    explicit shared_dereference( type* value )
        : shared_base< type, function, use_t >( value )
    {
    }
};

template< void ( * function )( const void* ), typename use_t >
    class shared_dereference< void, function, use_t >
    : public shared_base< void, function, use_t >
{
protected:
    explicit shared_dereference( void* value )
        : shared_base< void, function, use_t >( value )
    {
    }
};

//--- shared_ptr -----------------------------------------------------------------------------------
template< typename type >
    struct shared_ptr
    : public shared_dereference< type, deallocate_ptr< type >, unsigned >
{
    explicit shared_ptr( type* value = 0 )
        : shared_dereference< type, deallocate_ptr< type >, unsigned >( value )
    {
    }
};

//--- shared_array ---------------------------------------------------------------------------------
template< typename type >
    struct shared_array
    : public shared_dereference< type, deallocate_array< type >, unsigned >
{
    explicit shared_array( type* value = 0 )
        : shared_dereference< type, deallocate_array< type >, unsigned >( value )
    {
    }
};

//--- shared_free ----------------------------------------------------------------------------------
template< typename type >
    struct shared_free
    : public shared_dereference< type, deallocate_free, unsigned >
{
    explicit shared_free( type* value = 0 )
        : shared_dereference< type, deallocate_free, unsigned >( value )
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
        explicit opaque_ptr( t* value, function_type function = deallocate_ptr< t > )
        : data( new opaque_data( value, function ) )
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

    template< typename t >
        void reset( t* value, function_type function = deallocate_ptr< t > )
    {
        data->decrement();
        data = new opaque_data( value, function );
    }

private:
    opaque_data* data;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_UTILITY_POINTER_HPP
