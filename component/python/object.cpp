/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <python3.2m/Python.h>

#include "component/python/traits.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- is_none --------------------------------------------------------------------------------------
bool is_none( data* data )
{
    return data == Py_None;
}

//--- none -----------------------------------------------------------------------------------------
data* none( void )
{
    Py_RETURN_NONE;
}

//--- get_exception --------------------------------------------------------------------------------
exception_tuple get_exception( void )
{
    object py_type;
    object py_value;
    object py_backtrace;
    PyErr_Fetch( &py_type, &py_value, &py_backtrace );
    PyErr_NormalizeException( &py_type, &py_value, &py_backtrace );

    object str_type = py_type.str();
    object str_value = py_value.str();
    object str_backtrace = py_value.str();

    std::string type;
    std::string value;
    std::string backtrace;
    as< std::string >::call( str_type, type );
    as< std::string >::call( str_value, value );
    as< std::string >::call( str_backtrace, backtrace );

    return exception_tuple( type + ' ' + value, backtrace );
}

//--- set_exception --------------------------------------------------------------------------------
void set_exception( const c8* what, const c8* where )
{
    std::string string;

    if ( PyErr_Occurred() )
    {
        exception_tuple tuple = get_exception();
        string << tuple._0 << "\n\nStack trace: " << tuple._1;
    }

    string << what << "\n\nStack trace: " << where;
    PyErr_SetString( PyExc_RuntimeError, string.c_str() );
}

//--- object_base ----------------------------------------------------------------------------------
object_base::object_base( void )
    : data( 0 )
{
}

object_base::object_base( python::data* data_ )
    : data( data_ )
{
    if ( !data )
        throw error::python() << "Invalid python object";
}

object_base::operator python::data*( void ) const
{
    return data;
}

//--- object ---------------------------------------------------------------------------------------
object::object( void )
    : object_base()
{
}

object::object( python::data* data_ )
    : object_base( data_ )
{
}

object::object( const object& other )
    : object_base( other.data )
{
    Py_XINCREF( data );
}

object::~object( void )
{
    Py_XDECREF( data );
}

object& object::operator =( python::data* other )
{
    Py_XDECREF( data );
    data = other;
    return *this;
}

object& object::operator =( const object& other )
{
    Py_XINCREF( other.data );
    Py_XDECREF( data );
    data = other.data;
    return *this;
}

data** object::operator &( void )
{
    return &data;
}

data* object::release( void )
{
    python::data* pass_back = data;
    data = 0;
    return pass_back;
}

object object::str( void ) const
{
    return PyObject_Str( data );
}

//--- capsule --------------------------------------------------------------------------------------
capsule::capsule( void* pointer_, component::throw_type function_, destructor destructor )
    : object_base( PyCapsule_New( pointer_, 0, destructor ) )
{
    if ( PyCapsule_SetContext( data, ptr_cast( function_ ) ) )
    {
        Py_DECREF( data );
        throw error::python() << "Unable to set context of capsule";
    }
}

capsule::capsule( python::data* data_ )
    : object_base( data_ )
{
    if ( !PyCapsule_CheckExact( data ) )
        throw error::python() << "Object is not a capsule";
}

void* capsule::pointer( void ) const
{
    return PyCapsule_GetPointer( data, 0 );
}

component::throw_type capsule::function( void ) const
{
    return ptr_cast< component::throw_type >( PyCapsule_GetContext( data ) );
}

void capsule::reset( void )
{
    if ( PyCapsule_SetDestructor( data, 0 ) )
        throw error::python() << "Unable to set destructor of capsule";

    if ( PyCapsule_SetContext( data, 0 ) )
        throw error::python() << "Unable to set context of capsule";
}

//--- boolean --------------------------------------------------------------------------------------
boolean::boolean( bool value )
    : object_base( value ? Py_True : Py_False )
{
    Py_INCREF( data );
}

boolean::boolean( python::data* data_ )
    : object_base( data_ )
{
    if ( !PyBool_Check( data ) )
        throw error::python() << "Object is not a boolean";
}

bool boolean::get( void ) const
{
    return data == Py_True;
}

//--- integral -------------------------------------------------------------------------------------
integral::integral( s64 value )
    : object_base( PyLong_FromLong( value ) )
{
}

integral::integral( python::data* data_ )
    : object_base( data_ )
{
    if ( !PyLong_Check( data ) )
        throw error::python() << "Object is not a integral number";
}

s64 integral::get( void ) const
{
    return PyLong_AS_LONG( data );
}

//--- floating_point -------------------------------------------------------------------------------
floating_point::floating_point( f64 value )
    : object_base( PyFloat_FromDouble( value ) )
{
}

floating_point::floating_point( python::data* data_ )
    : object_base( data_ )
{
    if ( !PyFloat_Check( data ) )
        throw error::python() << "Object is not a floating-point number";
}

f64 floating_point::get( void ) const
{
    return PyFloat_AS_DOUBLE( data );
}

//--- string ---------------------------------------------------------------------------------------
string::string( const c8* string_, up_t size )
    : object_base( PyUnicode_FromStringAndSize( string_, size ) )
{
}

string::string( python::data* data_ )
    : object_base( data_ )
{
    if ( !PyUnicode_Check( data ) )
        throw error::python() << "Object is not a string";
}

string::tuple string::get( void ) const
{
    sp_t size;
    const c8* string_ = _PyUnicode_AsStringAndSize( data, &size );
    return make_tuple( string_, size );
}

//--- list -----------------------------------------------------------------------------------------
list::list( up_t size_ )
    : object_base( PyList_New( size_ ) )
{
}

list::list( python::data* data_ )
    : object_base( data_ )
{
    if ( !PyList_Check( data ) )
        throw error::python() << "Object is not a list";
}

up_t list::size( void ) const
{
    return PyList_GET_SIZE( data );
}

data* list::get( up_t index ) const
{
    return PyList_GET_ITEM( data, index );
}

void list::set( up_t index, python::data* item )
{
    PyList_SET_ITEM( data, index, item );
}

//--- iterator -------------------------------------------------------------------------------------
iterator::iterator( python::data* data_ )
    : object_base( data_ )
{
}

iterator::~iterator( void )
{
    Py_DECREF( data );
}

python::data* iterator::next( void ) const
{
    return PyIter_Next( data );
}

//--- set ------------------------------------------------------------------------------------------
set::set( void )
    : object_base( PySet_New( 0 ) )
{
}

set::set( python::data* data_ )
    : object_base( data_ )
{
    if( !PySet_Check( data ) )
        throw error::python() << "Object is not a set";
}

iterator set::get( void ) const
{
    return PyObject_GetIter( data );
}

void set::add( python::data* item )
{
    PySet_Add( data, item );
}

//--- dictionary -----------------------------------------------------------------------------------
dictionary::dictionary( void )
    : object_base( PyDict_New() )
{
}

dictionary::dictionary( python::data* data_ )
    : object_base( data_ )
{
    if ( !PyDict_Check( data ) )
        throw error::python() << "Object is not a dictionary";
}

bool dictionary::next( tuple& item ) const
{
    return PyDict_Next( data, &item._0, &item._1, &item._2 );
}

python::data* dictionary::get( python::data* key ) const
{
    return PyDict_GetItem( data, key );
}

void dictionary::set( python::data* key, python::data* value )
{
    if ( PyDict_SetItem( data, key, value ) )
        throw error::python() << "Unable to set dictionary item";
}

//--- tuple ----------------------------------------------------------------------------------------
tuple::tuple( up_t size_ )
    : object_base( PyTuple_New( size_ ) )
{
}

tuple::tuple( python::data* data_ )
    : object_base( data_ )
{
    if ( !PyTuple_Check( data ) )
        throw error::python() << "Object is not a tuple";
}

up_t tuple::size( void ) const
{
    return PyTuple_GET_SIZE( data );
}

python::data* tuple::get( up_t index ) const
{
    return PyTuple_GET_ITEM( data, index );
}

void tuple::set( up_t index, python::data* item )
{
    PyTuple_SET_ITEM( data, index, item );
}

//--- method ---------------------------------------------------------------------------------------
method::method( const c8* name, cfunction call, type flag )
    : pointer( new PyMethodDef )
{
    PyMethodDef& definition = *pointer.as< PyMethodDef >();
    std::memset( &definition, 0, sizeof( definition ) );

    definition.ml_name = name;
    definition.ml_meth = call;
    definition.ml_flags = flag == single ? METH_O : METH_VARARGS;
}

void* method::get( void ) const
{
    return pointer.get();
}

void method::set( const c8* name, const c8* doc )
{
    PyMethodDef& definition = *pointer.as< PyMethodDef >();
    definition.ml_name = name;
    definition.ml_doc = doc;
}

//--- function -------------------------------------------------------------------------------------
function::function( const method& method, python::data* self )
    : object_base( PyCFunction_New( static_cast< PyMethodDef* >( method.get() ), self ) )
{
}

//--- module ---------------------------------------------------------------------------------------
module::module( const c8* name, type flag )
    : object_base( flag == make ? PyModule_New( name ) : PyImport_AddModule( name ) )
{
}

data* module::get( void ) const
{
    return PyModule_GetDict( data );
}

void module::add( const c8* name, python::data* item )
{
    PyModule_AddObject( data, name, item );
}

OOE_NAMESPACE_END( ( ooe )( python ) )
