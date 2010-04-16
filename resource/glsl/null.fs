varying vec2 texcoord;

uniform struct vsampler2D
{
	float page_ratio;
	float page_log2;
	sampler2D page_cache;
	sampler2D page_table;
} vt;

vec4 vtexture2D( vsampler2D, vec2 );

void main( void )
{
	gl_FragColor = vtexture2D( vt, texcoord );
}
