varying vec2 texcoord;

vec4 vtexture2D( in vec2 );

void main( void )
{
	gl_FragData[ 0 ] = vtexture2D( texcoord );
}
