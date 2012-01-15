/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_FORK_IO_HPP
#define OOE_FOUNDATION_EXECUTABLE_FORK_IO_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

typedef tuple< s32 /* in */, s32 /* out */, s32 /* error */ > io_triplet;

//--- fork_id --------------------------------------------------------------------------------------
struct fork_id
{
    pid_t pid;

    fork_id( void );
    fork_id( const io_triplet& );
};

//--- fork_io --------------------------------------------------------------------------------------
class OOE_VISIBLE fork_io
{
public:
    enum wait_type
    {
        failure,
        success,
        waiting
    };

    fork_io( void );
    fork_io( const io_triplet& );
    ~fork_io( void );

    wait_type wait( bool = true ) const;
    void signal( s32 ) const;
    bool is_child( void ) const;

    static void execute( const std::string&, ... ) OOE_NORETURN OOE_SENTINEL;
    static void exit( bool ) OOE_NORETURN;

private:
    shared_ptr< const fork_id > id;
};

//--- scoped_fork ----------------------------------------------------------------------------------
struct OOE_VISIBLE scoped_fork
    : public fork_io
{
    scoped_fork( void );
    scoped_fork( const io_triplet& );
    ~scoped_fork( void );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_EXECUTABLE_FORK_IO_HPP
