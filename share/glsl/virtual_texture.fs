vec4 vtexture2D( vsampler2D sampler, vec2 virtual_address )
{
    // will default to loading first page if lookup fails to find valid data
    vec2 entry = vec2( 0, 1 );

    // handle page fault by ascending mipmap levels to find valid data
    for ( float bias = sampler.bias_range.x; bias != sampler.bias_range.y; ++bias )
    {
        entry = texture2D( sampler.page_table, virtual_address, bias ).xy;

        if ( entry.x > -1. )
            break;
    }

    vec2 physical_address = fract( virtual_address * entry.y );
    return texture2DArray( sampler.page_cache, vec3( physical_address, entry.x ) );
}
