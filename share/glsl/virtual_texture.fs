/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

vec3 vlookup( vsampler2D sampler, vec2 virtual )
{
    vec2 dx = dFdx( virtual );
    vec2 dy = dFdy( virtual );
    ivec2 entry = ivec2( -1, -1 );

    // handle page fault by ascending mipmap levels to find valid data
    for ( int bias = sampler.bias_range.x; entry.x == -1 && bias != sampler.bias_range.y; ++bias )
    {
        float scale = exp2( float( bias ) );
        entry = textureGrad( sampler.page_table, virtual, dx * scale, dy * scale ).xy;
    }

    float shift = exp2( float( entry.y ) );
    return vec3( fract( virtual * shift ), entry.x );
}

vec4 vtexture( vsampler2D sampler, vec2 virtual )
{
    vec3 physical = vlookup( sampler, virtual );
    return texture( sampler.page_cache, physical );
}

vec4 vtexelFetch( vsampler2D sampler, vec2 virtual )
{
    vec3 physical = vlookup( sampler, virtual );
    physical.xy *= exp2( float( sampler.bias_range.x ) );
    return texelFetch( sampler.page_cache, ivec3( physical ), 0 );
}
