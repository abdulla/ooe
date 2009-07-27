/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_ENGINE_INTERFACE_HPP
#define OOE_EXTERNAL_ENGINE_INTERFACE_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/signal.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
	typedef signal< void ( u32 ) > second_signal;
	typedef signal< void ( f32 ) > frame_signal;

	typedef slot< void ( u32 ) > second_slot;
	typedef slot< void ( f32 ) > frame_slot;

	struct event_queue;
	class scheduler;
	class service;
	class vfs;
	struct video;

	struct register_type
	{
		ooe::service& service;
		ooe::scheduler& scheduler;
		const event_queue& queue;
	};

	struct core_type
	{
		ooe::vfs& vfs;
		ooe::scheduler& scheduler;
		ooe::video& video;
	};

	struct settings
	{
		struct
		{
			std::string path;
			bool input;
			bool sight;
		} service;

		struct
		{
			u16 width;
			u16 height;
			bool full;
		} view;

		struct
		{
			std::string path;
			bool sync;
		} video;

		struct
		{
			u16 width;
			u16 height;
		} sight;
	};
}

#endif	// OOE_EXTERNAL_ENGINE_INTERFACE_HPP
