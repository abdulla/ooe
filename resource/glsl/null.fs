uniform sampler2D page_table;
uniform sampler2D page_cache;
varying vec2 texcoord;

vec4 virtual_texture( sampler2D, sampler2D, vec2 );

void main( void )
{
	gl_FragColor = virtual_texture( page_table, page_cache, texcoord );
}
