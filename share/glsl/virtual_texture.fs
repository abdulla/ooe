vec3 vaddress( vsampler2D sampler, vec2 virtual_address )
{
    vec2 dx = dFdx( virtual_address );
    vec2 dy = dFdy( virtual_address );
    ivec2 entry = ivec2( -1, -1 );

    // handle page fault by ascending mipmap levels to find valid data
    for ( int bias = sampler.bias_range.x; entry.x == -1 && bias != sampler.bias_range.y; ++bias )
    {
        float scale = exp2( float( -bias ) );
        entry = texture2DGrad( sampler.page_table, virtual_address, dx / scale, dy / scale ).xy;
    }

    // default to loading first page if lookup fails to find valid data
    if ( entry.x == -1  )
        entry = ivec2( 0, 0 );

    float shift = exp2( float( entry.y ) );
    return vec3( fract( virtual_address * shift ), entry.x );
}

vec4 vtexture2D( vsampler2D sampler, vec2 virtual_address )
{
    vec3 physical_address = vaddress( sampler, virtual_address );
    return texture2DArray( sampler.page_cache, physical_address );
}

vec4 vtexel2D( vsampler2D sampler, vec2 virtual_address )
{
    vec3 physical_address = vaddress( sampler, virtual_address );
    float page_size = exp2( float( sampler.bias_range.x ) );
    ivec3 index = ivec3( physical_address.xy * page_size, physical_address.z );
    return texelFetch2DArray( sampler.page_cache, index, 0 );
}
