vec4 virtual_texture( sampler2D page_table, sampler2D page_cache, vec2 virtual_address )
{
	// TODO: loop upwards if entry.z = -1
	// entry = { x: physical-x, y: physical-y, z: 2^mipmap-level }
	vec4 entry = texture2D( page_table, virtual_address );
	vec2 physical_address = entry.xy + fract( virtual_address * entry.z );
	return texture2D( page_cache, physical_address );
}
