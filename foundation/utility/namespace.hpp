/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_NAMESPACE_HPP
#define OOE_FOUNDATION_UTILITY_NAMESPACE_HPP

#include <boost/preprocessor/seq/for_each.hpp>

#define OOE_NAMESPACE_BEGIN_MACRO( r, _, e ) namespace e {
#define OOE_NAMESPACE_END_MACRO( r, _, e ) }
#define OOE_ANONYMOUS_NAMESPACE_MACRO( r, _, e ) using namespace e;

#define OOE_NAMESPACE_BEGIN( s ) BOOST_PP_SEQ_FOR_EACH( OOE_NAMESPACE_BEGIN_MACRO, ~, s )
#define OOE_NAMESPACE_END( s ) BOOST_PP_SEQ_FOR_EACH( OOE_NAMESPACE_END_MACRO, ~, s )

#define OOE_ANONYMOUS_NAMESPACE_BEGIN( s )\
    namespace { BOOST_PP_SEQ_FOR_EACH( OOE_ANONYMOUS_NAMESPACE_MACRO, ~, s )
#define OOE_ANONYMOUS_NAMESPACE_END( s ) }

#endif  // OOE_FOUNDATION_UTILITY_NAMESPACE_HPP
