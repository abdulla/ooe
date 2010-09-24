#extension GL_EXT_gpu_shader4 : enable

struct vsampler2D
{
	float page_log2;
	sampler2DArray page_cache;
	sampler2D page_table;
};

vec4 vtexture2D( vsampler2D, vec2 );
uniform vsampler2D vt;
varying vec2 texcoord;

void main( void )
{
	gl_FragColor = vtexture2D( vt, texcoord );
}
