#extension GL_EXT_gpu_shader4 : enable

struct vsampler2D
{
	vec2 bias_range;
	sampler2DArray page_cache;
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
