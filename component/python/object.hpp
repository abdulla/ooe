/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_PYTHON_OBJECT_HPP
#define OOE_COMPONENT_PYTHON_OBJECT_HPP

#include "component/registry/traits.hpp"

struct _object;

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

typedef _object data;
typedef data* ( * cfunction )( data*, data* );
typedef void ( * destructor )( data* );
typedef tuple< std::string /* value */, std::string /* backtrace */ > exception_tuple;

bool is_none( data* ) OOE_VISIBLE;
data* none( void ) OOE_VISIBLE;
exception_tuple get_exception( void ) OOE_VISIBLE;
void set_exception( const c8*, const c8* ) OOE_VISIBLE;

//--- object_base ----------------------------------------------------------------------------------
class object_base
{
public:
    operator python::data*( void ) const OOE_VISIBLE;

protected:
    python::data* data;

    object_base( void );
    object_base( python::data* );
};

//--- object ---------------------------------------------------------------------------------------
struct OOE_VISIBLE object
    : public object_base
{
    object( void );
    object( python::data* );
    object( const object& );
    ~object( void );

    object& operator =( python::data* );
    object& operator =( const object& );
    python::data** operator &( void );

    python::data* release( void );
    object str( void ) const OOE_HIDDEN;
};

//--- capsule --------------------------------------------------------------------------------------
struct OOE_VISIBLE capsule
    : public object_base
{
    capsule( void*, component::throw_type, destructor = 0 );
    capsule( python::data* );

    void* pointer( void ) const;
    component::throw_type function( void ) const;

    void reset( void );
};

//--- boolean --------------------------------------------------------------------------------------
struct OOE_VISIBLE boolean
    : public object_base
{
    boolean( bool );
    boolean( python::data* );

    bool get( void ) const;
};

//--- integral -------------------------------------------------------------------------------------
struct OOE_VISIBLE integral
    : public object_base
{
    integral( s64 );
    integral( python::data* );

    s64 get( void ) const;
};

//--- floating_point -------------------------------------------------------------------------------
struct OOE_VISIBLE floating_point
    : public object_base
{
    floating_point( f64 );
    floating_point( python::data* );

    f64 get( void ) const;
};

//--- string ---------------------------------------------------------------------------------------
struct OOE_VISIBLE string
    : public object_base
{
    typedef ooe::tuple< const c8*, up_t > tuple;

    string( const c8*, up_t );
    string( python::data* );

    tuple get( void ) const;
};

//--- list -----------------------------------------------------------------------------------------
struct OOE_VISIBLE list
    : public object_base
{
    list( up_t );
    list( python::data* );

    up_t size( void ) const;

    python::data* get( up_t ) const;
    void set( up_t, python::data* );
};

//--- iterator -------------------------------------------------------------------------------------
struct OOE_VISIBLE iterator
    : public object_base
{
    iterator( python::data* ) OOE_HIDDEN;
    ~iterator( void );

    python::data* next( void ) const;
};

//--- set ------------------------------------------------------------------------------------------
struct OOE_VISIBLE set
    : public object_base
{
    set( void );
    set( python::data* );

    iterator get( void ) const;
    void add( python::data* );
};

//--- dictionary -----------------------------------------------------------------------------------
struct OOE_VISIBLE dictionary
    : public object_base
{
    typedef ooe::tuple< sp_t, python::data*, python::data* > tuple;

    dictionary( void );
    dictionary( python::data* );

    bool next( tuple& ) const;
    python::data* get( python::data* ) const OOE_HIDDEN;
    void set( python::data*, python::data* );
};

//--- tuple ----------------------------------------------------------------------------------------
struct OOE_VISIBLE tuple
    : public object_base
{
    tuple( up_t );
    tuple( python::data* );

    up_t size( void ) const;

    python::data* get( up_t ) const;
    void set( up_t, python::data* );
};

//--- method ---------------------------------------------------------------------------------------
class method
{
public:
    enum type
    {
        single,
        multiple
    };

    method( const c8*, cfunction, type );

    void* get( void ) const;
    void set( const c8*, const c8* );

private:
    opaque_ptr pointer;
};

//--- function -------------------------------------------------------------------------------------
struct function
    : public object_base
{
    function( const method&, python::data* );
};

//--- module ---------------------------------------------------------------------------------------
struct module
    : public object_base
{
    enum type
    {
        make,
        import
    };

    module( const c8*, type );

    python::data* get( void ) const;
    void add( const c8*, python::data* );
};

OOE_NAMESPACE_END( ( ooe )( python ) )

#endif  // OOE_COMPONENT_PYTHON_OBJECT_HPP
