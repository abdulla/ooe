/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_PROGRAM_HPP
#define OOE_FOUNDATION_EXECUTABLE_PROGRAM_HPP

#include <csignal>

#include "foundation/executable/program_forward.hpp"
#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( executable ) )

typedef void ( * signal_handler_type )( s32, siginfo_t*, void* );
typedef bool ( * launch_type )( const std::string&, const std::string&, s32, c8** );
typedef tuple< std::string, std::string > path_type;

signal_handler_type signal( struct sigaction&, signal_handler_type, s32 ) OOE_VISIBLE;
s32 has_signal( void ) OOE_VISIBLE;
void quit( void ) OOE_VISIBLE;

s32 launch( launch_type, s32, c8** ) OOE_VISIBLE;
void null_fd( s32 ) OOE_VISIBLE;
path_type path( void ) OOE_VISIBLE;
bool path( c8*, up_t );

OOE_NAMESPACE_END( ( ooe )( executable ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

bool launch( executable::launch_type, const std::string&, const std::string&, s32, c8** );

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_FOUNDATION_EXECUTABLE_PROGRAM_HPP
