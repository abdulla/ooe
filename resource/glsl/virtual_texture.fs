struct vsampler2D
{
	float page_ratio;
	float page_log2;
	sampler2D page_cache;
	sampler2D page_table;
};

vec4 vtexture2D( vsampler2D sampler, vec2 virtual_address )
{
	vec4 entry;

	// handle page fault by ascending mipmap levels to find valid data
	do
		entry = texture2D( sampler.page_table, virtual_address, sampler.page_log2++ );
	while ( entry.x < 0. );

	vec2 physical_address = entry.xy + fract( virtual_address * entry.z ) * sampler.page_ratio;
	return texture2D( sampler.page_cache, physical_address );
}
