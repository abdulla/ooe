#extension GL_EXT_gpu_shader4 : enable

struct vsampler2D
{
	float page_log2;
	sampler2DArray page_cache;
	sampler2D page_table;
};

vec4 vtexture2D( vsampler2D sampler, vec2 virtual_address )
{
	vec4 entry;

	// handle page fault by ascending mipmap levels to find valid data
	do
		entry = texture2D( sampler.page_table, virtual_address, sampler.page_log2++ );
	while ( entry.x < 0. );

	vec2 physical_address = fract( virtual_address * entry.y );
	return texture2DArray( sampler.page_cache, vec3( physical_address, entry.x ) );
}
