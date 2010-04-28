struct vsampler2D
{
	float page_ratio;
	float page_log2;
	sampler2D page_cache;
	sampler2D page_table;
};

vec4 vtexture2D( vsampler2D, vec2 );
uniform vsampler2D vt;
uniform ivec3 colour;
varying vec2 texcoord;

void main( void )
{
	gl_FragColor = vec4( colour / 255, vtexture2D( vt, texcoord ).a );
}
