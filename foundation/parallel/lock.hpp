/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_PARALLEL_LOCK_HPP
#define OOE_FOUNDATION_PARALLEL_LOCK_HPP

#include <pthread.h>

#include "foundation/utility/noncopyable.hpp"
#include "foundation/utility/preprocessor.hpp"

#include BOOST_PP_STRINGIZE( foundation/parallel/OOE_PLATFORM/lock_forward.hpp )

OOE_NAMESPACE_BEGIN( ( ooe ) )

class lock;

//--- mutex ----------------------------------------------------------------------------------------
class OOE_VISIBLE mutex
    : private noncopyable
{
public:
    mutex( void );
    virtual ~mutex( void );

protected:
    pthread_mutex_t pthread_mutex;

    friend class condition;
    friend class lock;
};

//--- condition ------------------------------------------------------------------------------------
class OOE_VISIBLE condition
    : private noncopyable
{
public:
    condition( void );
    virtual ~condition( void );

    void wait( lock& );
    void notify_one( void );
    void notify_all( void );

private:
    pthread_cond_t pthread_cond;
};

//--- read_write -----------------------------------------------------------------------------------
class OOE_VISIBLE read_write
    : private noncopyable
{
public:
    read_write( void );
    ~read_write( void );

private:
    pthread_rwlock_t pthread_rwlock;

    friend class read_lock;
    friend class write_lock;
};

//--- semaphore ------------------------------------------------------------------------------------
struct OOE_VISIBLE semaphore
    : private platform::semaphore
{
    semaphore( u32 );
    ~semaphore( void );

    void up( void );
    void down( void );
};

//--- lock -----------------------------------------------------------------------------------------
class OOE_VISIBLE lock
    : private noncopyable
{
public:
    lock( ooe::mutex& );
    ~lock( void );

private:
    ooe::mutex& mutex;

    friend class condition;
};

//--- read_lock ------------------------------------------------------------------------------------
class OOE_VISIBLE read_lock
    : private noncopyable
{
public:
    read_lock( ooe::read_write& );
    ~read_lock( void );

private:
    ooe::read_write& read_write;
};

//--- write_lock -----------------------------------------------------------------------------------
class OOE_VISIBLE write_lock
    : private noncopyable
{
public:
    write_lock( ooe::read_write& );
    ~write_lock( void );

private:
    ooe::read_write& read_write;
};

//--- semaphore_lock -------------------------------------------------------------------------------
class OOE_VISIBLE semaphore_lock
{
public:
    semaphore_lock( ooe::semaphore& );
    ~semaphore_lock( void );

private:
    ooe::semaphore& semaphore;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_PARALLEL_LOCK_HPP
