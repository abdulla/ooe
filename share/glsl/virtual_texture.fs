vec4 vtexture2D( vsampler2D sampler, vec2 virtual_address )
{
    ivec2 entry = ivec2( -1, -1 );

    // handle page fault by ascending mipmap levels to find valid data
    for ( int bias = sampler.bias_range.x; entry.x == -1 && bias != sampler.bias_range.y; ++bias )
        entry = texture2D( sampler.page_table, virtual_address, float( bias ) ).xy;

    // default to loading first page if lookup fails to find valid data
    if ( entry.x == -1  )
        entry = ivec2( 0, 0 );

    vec2 physical_address = fract( virtual_address * exp2( float( entry.y ) ) );
    return texture2DArray( sampler.page_cache, vec3( physical_address, entry.x ) );
}
