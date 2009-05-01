/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_ENGINE_SETUP_HPP
#define OOE_EXTERNAL_ENGINE_SETUP_HPP

#include "foundation/utility/fundamental.hpp"

namespace ooe
{
	class descriptor;
	class dynamic;
	struct settings;
	struct video;
	class vfs;

	struct path
	{
		std::string root;
		std::string vfs;
		std::string settings;
		std::string runtime;
	};

	bool setup_option( path&, s32, c8** );
	void setup_vfs( vfs&, const path& );
	void setup_settings( settings&, const descriptor& );
	void setup_video( video&, u16, u16, bool );
}

#endif	// OOE_EXTERNAL_ENGINE_SETUP_HPP
