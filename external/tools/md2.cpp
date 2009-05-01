/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/io/memory.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"

#include "external/tools/interface.hpp"
#include "external/tools/md2.hpp"

namespace
{
	using namespace ooe;

	const f32 normal[][ 3 ] =
	{
		{ -0.525731f,  0.000000f,  0.850651f },
		{ -0.442863f,  0.238856f,  0.864188f },
		{ -0.295242f,  0.000000f,  0.955423f },
		{ -0.309017f,  0.500000f,  0.809017f },
		{ -0.162460f,  0.262866f,  0.951056f },
		{  0.000000f,  0.000000f,  1.000000f },
		{  0.000000f,  0.850651f,  0.525731f },
		{ -0.147621f,  0.716567f,  0.681718f },
		{  0.147621f,  0.716567f,  0.681718f },
		{  0.000000f,  0.525731f,  0.850651f },
		{  0.309017f,  0.500000f,  0.809017f },
		{  0.525731f,  0.000000f,  0.850651f },
		{  0.295242f,  0.000000f,  0.955423f },
		{  0.442863f,  0.238856f,  0.864188f },
		{  0.162460f,  0.262866f,  0.951056f },
		{ -0.681718f,  0.147621f,  0.716567f },
		{ -0.809017f,  0.309017f,  0.500000f },
		{ -0.587785f,  0.425325f,  0.688191f },
		{ -0.850651f,  0.525731f,  0.000000f },
		{ -0.864188f,  0.442863f,  0.238856f },
		{ -0.716567f,  0.681718f,  0.147621f },
		{ -0.688191f,  0.587785f,  0.425325f },
		{ -0.500000f,  0.809017f,  0.309017f },
		{ -0.238856f,  0.864188f,  0.442863f },
		{ -0.425325f,  0.688191f,  0.587785f },
		{ -0.716567f,  0.681718f, -0.147621f },
		{ -0.500000f,  0.809017f, -0.309017f },
		{ -0.525731f,  0.850651f,  0.000000f },
		{  0.000000f,  0.850651f, -0.525731f },
		{ -0.238856f,  0.864188f, -0.442863f },
		{  0.000000f,  0.955423f, -0.295242f },
		{ -0.262866f,  0.951056f, -0.162460f },
		{  0.000000f,  1.000000f,  0.000000f },
		{  0.000000f,  0.955423f,  0.295242f },
		{ -0.262866f,  0.951056f,  0.162460f },
		{  0.238856f,  0.864188f,  0.442863f },
		{  0.262866f,  0.951056f,  0.162460f },
		{  0.500000f,  0.809017f,  0.309017f },
		{  0.238856f,  0.864188f, -0.442863f },
		{  0.262866f,  0.951056f, -0.162460f },
		{  0.500000f,  0.809017f, -0.309017f },
		{  0.850651f,  0.525731f,  0.000000f },
		{  0.716567f,  0.681718f,  0.147621f },
		{  0.716567f,  0.681718f, -0.147621f },
		{  0.525731f,  0.850651f,  0.000000f },
		{  0.425325f,  0.688191f,  0.587785f },
		{  0.864188f,  0.442863f,  0.238856f },
		{  0.688191f,  0.587785f,  0.425325f },
		{  0.809017f,  0.309017f,  0.500000f },
		{  0.681718f,  0.147621f,  0.716567f },
		{  0.587785f,  0.425325f,  0.688191f },
		{  0.955423f,  0.295242f,  0.000000f },
		{  1.000000f,  0.000000f,  0.000000f },
		{  0.951056f,  0.162460f,  0.262866f },
		{  0.850651f, -0.525731f,  0.000000f },
		{  0.955423f, -0.295242f,  0.000000f },
		{  0.864188f, -0.442863f,  0.238856f },
		{  0.951056f, -0.162460f,  0.262866f },
		{  0.809017f, -0.309017f,  0.500000f },
		{  0.681718f, -0.147621f,  0.716567f },
		{  0.850651f,  0.000000f,  0.525731f },
		{  0.864188f,  0.442863f, -0.238856f },
		{  0.809017f,  0.309017f, -0.500000f },
		{  0.951056f,  0.162460f, -0.262866f },
		{  0.525731f,  0.000000f, -0.850651f },
		{  0.681718f,  0.147621f, -0.716567f },
		{  0.681718f, -0.147621f, -0.716567f },
		{  0.850651f,  0.000000f, -0.525731f },
		{  0.809017f, -0.309017f, -0.500000f },
		{  0.864188f, -0.442863f, -0.238856f },
		{  0.951056f, -0.162460f, -0.262866f },
		{  0.147621f,  0.716567f, -0.681718f },
		{  0.309017f,  0.500000f, -0.809017f },
		{  0.425325f,  0.688191f, -0.587785f },
		{  0.442863f,  0.238856f, -0.864188f },
		{  0.587785f,  0.425325f, -0.688191f },
		{  0.688191f,  0.587785f, -0.425325f },
		{ -0.147621f,  0.716567f, -0.681718f },
		{ -0.309017f,  0.500000f, -0.809017f },
		{  0.000000f,  0.525731f, -0.850651f },
		{ -0.525731f,  0.000000f, -0.850651f },
		{ -0.442863f,  0.238856f, -0.864188f },
		{ -0.295242f,  0.000000f, -0.955423f },
		{ -0.162460f,  0.262866f, -0.951056f },
		{  0.000000f,  0.000000f, -1.000000f },
		{  0.295242f,  0.000000f, -0.955423f },
		{  0.162460f,  0.262866f, -0.951056f },
		{ -0.442863f, -0.238856f, -0.864188f },
		{ -0.309017f, -0.500000f, -0.809017f },
		{ -0.162460f, -0.262866f, -0.951056f },
		{  0.000000f, -0.850651f, -0.525731f },
		{ -0.147621f, -0.716567f, -0.681718f },
		{  0.147621f, -0.716567f, -0.681718f },
		{  0.000000f, -0.525731f, -0.850651f },
		{  0.309017f, -0.500000f, -0.809017f },
		{  0.442863f, -0.238856f, -0.864188f },
		{  0.162460f, -0.262866f, -0.951056f },
		{  0.238856f, -0.864188f, -0.442863f },
		{  0.500000f, -0.809017f, -0.309017f },
		{  0.425325f, -0.688191f, -0.587785f },
		{  0.716567f, -0.681718f, -0.147621f },
		{  0.688191f, -0.587785f, -0.425325f },
		{  0.587785f, -0.425325f, -0.688191f },
		{  0.000000f, -0.955423f, -0.295242f },
		{  0.000000f, -1.000000f,  0.000000f },
		{  0.262866f, -0.951056f, -0.162460f },
		{  0.000000f, -0.850651f,  0.525731f },
		{  0.000000f, -0.955423f,  0.295242f },
		{  0.238856f, -0.864188f,  0.442863f },
		{  0.262866f, -0.951056f,  0.162460f },
		{  0.500000f, -0.809017f,  0.309017f },
		{  0.716567f, -0.681718f,  0.147621f },
		{  0.525731f, -0.850651f,  0.000000f },
		{ -0.238856f, -0.864188f, -0.442863f },
		{ -0.500000f, -0.809017f, -0.309017f },
		{ -0.262866f, -0.951056f, -0.162460f },
		{ -0.850651f, -0.525731f,  0.000000f },
		{ -0.716567f, -0.681718f, -0.147621f },
		{ -0.716567f, -0.681718f,  0.147621f },
		{ -0.525731f, -0.850651f,  0.000000f },
		{ -0.500000f, -0.809017f,  0.309017f },
		{ -0.238856f, -0.864188f,  0.442863f },
		{ -0.262866f, -0.951056f,  0.162460f },
		{ -0.864188f, -0.442863f,  0.238856f },
		{ -0.809017f, -0.309017f,  0.500000f },
		{ -0.688191f, -0.587785f,  0.425325f },
		{ -0.681718f, -0.147621f,  0.716567f },
		{ -0.442863f, -0.238856f,  0.864188f },
		{ -0.587785f, -0.425325f,  0.688191f },
		{ -0.309017f, -0.500000f,  0.809017f },
		{ -0.147621f, -0.716567f,  0.681718f },
		{ -0.425325f, -0.688191f,  0.587785f },
		{ -0.162460f, -0.262866f,  0.951056f },
		{  0.442863f, -0.238856f,  0.864188f },
		{  0.162460f, -0.262866f,  0.951056f },
		{  0.309017f, -0.500000f,  0.809017f },
		{  0.147621f, -0.716567f,  0.681718f },
		{  0.000000f, -0.525731f,  0.850651f },
		{  0.425325f, -0.688191f,  0.587785f },
		{  0.587785f, -0.425325f,  0.688191f },
		{  0.688191f, -0.587785f,  0.425325f },
		{ -0.955423f,  0.295242f,  0.000000f },
		{ -0.951056f,  0.162460f,  0.262866f },
		{ -1.000000f,  0.000000f,  0.000000f },
		{ -0.850651f,  0.000000f,  0.525731f },
		{ -0.955423f, -0.295242f,  0.000000f },
		{ -0.951056f, -0.162460f,  0.262866f },
		{ -0.864188f,  0.442863f, -0.238856f },
		{ -0.951056f,  0.162460f, -0.262866f },
		{ -0.809017f,  0.309017f, -0.500000f },
		{ -0.864188f, -0.442863f, -0.238856f },
		{ -0.951056f, -0.162460f, -0.262866f },
		{ -0.809017f, -0.309017f, -0.500000f },
		{ -0.681718f,  0.147621f, -0.716567f },
		{ -0.681718f, -0.147621f, -0.716567f },
		{ -0.850651f,  0.000000f, -0.525731f },
		{ -0.688191f,  0.587785f, -0.425325f },
		{ -0.587785f,  0.425325f, -0.688191f },
		{ -0.425325f,  0.688191f, -0.587785f },
		{ -0.425325f, -0.688191f, -0.587785f },
		{ -0.587785f, -0.425325f, -0.688191f },
		{ -0.688191f, -0.587785f, -0.425325f }
	};

	namespace model
	{
		struct header
		{
			u32 id;					// fourcc: "IDP2"
			u32 version;			// version: 8

			u32 skin_width;			// texture width
			u32 skin_height;		// texture height

			u32 frame_size;			// size in bytes of a frame

			u32 length_skins;		// number of skins
			u32 length_vertices;	// number of vertices per frame
			u32 length_coords;		// number of texture coordinates
			u32 length_triangles;	// number of triangles
			u32 length_commands;	// number of opengl commands
			u32 length_frames;		// number of frames

			u32 offset_skins;		// offset to skins
			u32 offset_coords;		// offset to texture coordinates
			u32 offset_triangles;	// offset to triangles
			u32 offset_frames;		// offset to frames
			u32 offset_commands;	// offset to opengl commands
			u32 offset_end;			// offset to end of file
		};

		struct skin
		{
			c8 name[ 64 ];			// texture file name
		};

		struct coord
		{
			u16 u;
			u16 v;
		};

		struct triangle
		{
			u16 vertex[ 3 ];
			u16 coord[ 3 ];
		};

		struct vertex
		{
			u8 point[ 3 ];
			u8 normal;
		};

		struct frame
		{
			f32 scale[ 3 ];
			f32 translate[ 3 ];
			c8 name[ 16 ];
			model::vertex vertex[ 1 ];
		};

		struct command
		{
			f32 u;
			f32 v;
			s32 index;
		};
	}
}

namespace ooe
{
//--- make_md2 -----------------------------------------------------------------
	void make_md2( const descriptor& desc, u32 index, const f32* scale, const std::string& name )
	{
		memory memory( desc );
		void* base = memory.get();
		const model::header& header = *static_cast< model::header* >( base );

		if ( header.id != OOE_FOURCC( 'I', 'D', 'P', '2' ) || header.version != 8 )
			throw error::runtime( "md2: " ) << "Invalid MD2 model";
		else if ( index > header.length_frames )
			throw error::runtime( "md2: " ) << "Frame index out of range";

		const model::coord* coord = add< model::coord >( base, header.offset_coords );
		const model::triangle* triangle = add< model::triangle >( base, header.offset_triangles );
		const up_t vertex_size = ( header.length_vertices - 1 ) * sizeof( model::vertex );
		const up_t offset = index * ( sizeof( model::frame ) + vertex_size );
		const model::frame& frame =	*add< model::frame >( base, header.offset_frames + offset );

		file file( descriptor( name + ".lua", descriptor::write_new ) );
		const up_t length = header.length_triangles * 3;

		std::string string;
		string <<
		"function load( video )\n"
			"\tlocal layout_array =\n"
			"\t{\n"
				"\t\tvideo.vertex,\n"
				"\t\tvideo.normal,\n"
				"\t\tvideo.texture\n"
			"\t}\n"
			"\n"
			"\tlocal point_buffer = video:buffer( video.point )\n"
			"\tpoint_buffer:load( point_data )\n"
			"\n"
			"\treturn\n"
			"\t{\n"
				"\t\tmode = video.triangle_list,\n"
				"\t\tlength = " << length << ",\n"
				"\t\tlayout = layout_array,\n"
				"\t\tpoint = point_buffer\n"
			"\t}\n"
		"end\n"
		"\n"
		"point_data = \"";
		file.write( string.c_str(), string.size() );

		for ( u32 i = 0; i != header.length_triangles; ++i )
		{
			for ( u32 j = 0; j != 3; ++j )
			{
				const model::vertex& vertex = frame.vertex[ triangle[ i ].vertex[ j ] ];
				const model::coord& texture = coord[ triangle[ i ].coord[ j ] ];

				f32 point[] =
				{
					( frame.scale[ 0 ] * static_cast< f32 >( vertex.point[ 0 ] ) +
						frame.translate[ 0 ] ) * scale[ 0 ],
					( frame.scale[ 1 ] * static_cast< f32 >( vertex.point[ 1 ] ) +
						frame.translate[ 1 ] ) * scale[ 1 ],
					( frame.scale[ 2 ] * static_cast< f32 >( vertex.point[ 2 ] ) +
						frame.translate[ 2 ] ) * scale[ 2 ],

					normal[ vertex.normal ][ 0 ],
					normal[ vertex.normal ][ 1 ],
					normal[ vertex.normal ][ 2 ],

					divide( texture.u, header.skin_width ),
					divide( texture.v, header.skin_height )
				};

				tools::encode_write( file, point, sizeof( point ) );
			}
		}

		string.clear();
		string << '\"';
		file.write( string.c_str(), string.size() );
	}
}
