/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_FUNDAMENTAL_HPP
#define OOE_FOUNDATION_IPC_FUNDAMENTAL_HPP

#include "foundation/utility/fundamental.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

typedef u32 index_t;
typedef u32 bool_t;
typedef u32 error_t;
typedef u32 length_t;

/***************************************************************************************************
 * ________________________
 * MEMORY: ARGUMENTS HEADER
 *
 * +-----------+------------+-------
 * | 1: bool_t | 2: index_t | 3: ...
 * +-----------+------------+-------
 * 1: Describes whether the data is stored internally, or externally in additional shared memory.
 * 2: The index of the function to execute.
 * 3: This is the data to pass to the function, if it is stored internally; or the name of the
 * shared memory where the data is kept, if it is stored externally.
 *
 * ______________________
 * MEMORY: RESULTS HEADER
 *
 * +-----------+------------+-------
 * | 1: bool_t | 2: error_t | 3: ...
 * +-----------+------------+-------
 * 1: Describes whether the data is stored internally, or externally in additional shared memory.
 * 2: Indicates whether there was an error, and if so what type of error.
 * 3: This is the data for the result of the function, or the error strings if an error occurred.
 *
 * NOTE: There is an assumption in the code that index_t and error_t are the same size.
 *
 **************************************************************************************************/

/***************************************************************************************************
 * ________________________
 * SOCKET: ARGUMENTS HEADER
 *
 * +-------------+------------+-------
 * | 1: length_t | 2: index_t | 3: ...
 * +-------------+------------+-------
 * 1: The length of the additional data.
 * 2: The index of the function to call.
 * 3: The rest of the data to pass to the function.
 *
 * ________________________
 * SOCKET: RESULTS HEADER
 *
 * +-------------+------------+-------
 * | 1: length_t | 2: error_t | 3: ...
 * +-------------+------------+-------
 * 1: The length of the additional data.
 * 2: Indication of whether the data is the result, or an error message.
 * 3: The rest of the data, either the result of the function, or an error message.
 *
 * NOTE: There is an assumption in the code that index_t and error_t are the same size.
 **************************************************************************************************/

OOE_NAMESPACE_END( ( ooe )( ipc ) )

#endif  // OOE_FOUNDATION_IPC_FUNDAMENTAL_HPP
