vec3 vlookup( vsampler2D sampler, vec2 virtual )
{
    vec2 dx = dFdx( virtual );
    vec2 dy = dFdy( virtual );
    ivec2 entry = ivec2( -1, -1 );

    // handle page fault by ascending mipmap levels to find valid data
    for ( int bias = sampler.bias_range.x; entry.x == -1 && bias != sampler.bias_range.y; ++bias )
    {
        float scale = exp2( float( bias ) );
        entry = texture2DGrad( sampler.page_table, virtual, dx * scale, dy * scale ).xy;
    }

    // default to loading first page if lookup fails to find valid data
    if ( entry.x == -1  )
        entry = ivec2( 0, 0 );

    float shift = exp2( float( entry.y ) );
    return vec3( fract( virtual * shift ), entry.x );
}

vec4 vtexture2D( vsampler2D sampler, vec2 virtual )
{
    vec3 physical = vlookup( sampler, virtual );
    return texture2DArray( sampler.page_cache, physical );
}

vec4 vtexel2D( vsampler2D sampler, vec2 virtual )
{
    vec3 physical = vlookup( sampler, virtual );
    float page_size = exp2( float( sampler.bias_range.x ) );
    ivec3 index = ivec3( physical.xy * page_size, physical.z );
    return texelFetch2DArray( sampler.page_cache, index, 0 );
}
