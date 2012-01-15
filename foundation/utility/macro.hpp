/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_MACRO_HPP
#define OOE_FOUNDATION_UTILITY_MACRO_HPP

#if defined( __GNUC__ ) || defined( __clang__ )
    #define OOE_INLINE                      __attribute__( ( __always_inline__ ) )
    #define OOE_NOINLINE                    __attribute__( ( __noinline__ ) )
    #define OOE_NORETURN                    __attribute__( ( __noreturn__ ) )
    #define OOE_NONNULL                     __attribute__( ( __nonnull__ ) )
    #define OOE_SENTINEL                    __attribute__( ( __sentinel__ ) )
    #define OOE_WARNRESULT                  __attribute__( ( __warn_unused_result__ ) )
    #define OOE_PURE                        __attribute__( ( __pure__ ) )
    #define OOE_CONST                       __attribute__( ( __const__ ) )
    #define OOE_PACKED                      __attribute__( ( __packed__ ) )
    #define OOE_WEAK                        __attribute__( ( __weak__ ) )
    #define OOE_VISIBLE                     __attribute__( ( __visibility__( "default" ) ) )
    #define OOE_HIDDEN                      __attribute__( ( __visibility__( "hidden" ) ) )
    #define OOE_LIKELY( boolean )           __builtin_expect( ( boolean ), 1 )
    #define OOE_UNLIKELY( boolean )         __builtin_expect( ( boolean ), 0 )
#else
    #define OOE_INLINE
    #define OOE_NOINLINE
    #define OOE_NORETURN
    #define OOE_NONNULL
    #define OOE_SENTINEL
    #define OOE_WARNRESULT
    #define OOE_PURE
    #define OOE_CONST
    #define OOE_PACKED
    #define OOE_WEAK
    #define OOE_VISIBLE
    #define OOE_HIDDEN
    #define OOE_LIKELY( boolean )
    #define OOE_UNLIKELY( boolean )
#endif

#define OOE_FOURCC( a, b, c, d ) ( a + ( b << 8 ) + ( c << 16 ) + ( d << 24 ) )
#define OOE_EXPAND_END( text ) #text
#define OOE_EXPAND( text ) OOE_EXPAND_END( text )
#define OOE_PATH( path, file ) OOE_EXPAND( path/OOE_PLATFORM/file )


#define OOE_CONSOLE( statement )\
do\
{\
    std::cerr << statement << '\n';\
    std::cout << statement << '\n';\
}\
while ( false )

#define OOE_IGNORE( statement )\
do\
{\
    try\
    {\
        statement;\
    }\
    catch ( ... )\
    {\
    }\
}\
while ( false )

#define OOE_PRINT( id, statement )\
do\
{\
    try\
    {\
        statement;\
    }\
    catch ( error::runtime& error )\
    {\
        OOE_CONSOLE( id ": " <<\
                "Uncaught exception:\n" << error.what() << "\n\nStack trace:" << error.where() );\
    }\
    catch ( std::exception& error )\
    {\
        OOE_CONSOLE( id ": " << "Uncaught exception:\n" << error.what() );\
    }\
    catch ( ... )\
    {\
        OOE_CONSOLE( id ": " << "Uncaught exception:\nAn unknown exception was thrown\n" );\
    }\
}\
while ( false )

#endif  // OOE_FOUNDATION_UTILITY_MACRO_HPP
