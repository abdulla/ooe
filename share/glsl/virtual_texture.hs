/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

struct vsampler2D
{
    ivec2 bias_range;
    isampler2D page_table;
    sampler2DArray page_cache;
};

vec4 vtexture2D( vsampler2D, vec2 );
vec4 vtexel2D( vsampler2D, vec2 );
