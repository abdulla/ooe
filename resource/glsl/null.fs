uniform sampler2D texture;
varying vec2 texcoord;

void main( void )
{
	gl_FragData[ 0 ] = vec4( texcoord.xy, 1, 1. - texture2D( texture, texcoord ).a );
}
