varying float texel;
varying vec2 position;

void main( void )
{
	gl_FragColor = vec4( position.x, texel, position.y, 1 );
}
