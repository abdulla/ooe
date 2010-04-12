varying vec2 texcoord;

uniform struct virtual_texture
{
	sampler2D page_table;
} vt;

uniform struct physical_cache
{
	float page_ratio;
	float page_log2;
	sampler2D page_cache;
} cache;

vec4 vtexture2D( virtual_texture, physical_cache, vec2 );

void main( void )
{
	gl_FragColor = vtexture2D( vt, cache, texcoord );
}
