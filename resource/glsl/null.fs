uniform sampler2D texture;
varying vec2 texcoord;

void main( void )
{
	gl_FragData[ 0 ] = texture2D( texture, texcoord );
}
