struct virtual_texture
{
	sampler2D page_table;
};

struct physical_cache
{
	float page_ratio;
	float page_log2;
	sampler2D page_cache;
};

vec4 vtexture2D( virtual_texture vt, physical_cache cache, vec2 virtual_address )
{
	// entry = { x: physical-x, y: physical-y, z: 2^{ max-level - mipmap-level } }
	vec4 entry = texture2D( vt.page_table, virtual_address, cache.page_log2 - .5 );
	vec2 physical_address = entry.xy + fract( virtual_address * entry.z ) * cache.page_ratio;
	return texture2D( cache.page_cache, physical_address );
}
