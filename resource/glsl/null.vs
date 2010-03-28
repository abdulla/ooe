attribute vec2 vertex;
attribute vec2 coords;
uniform mat4 projection;
varying vec2 texcoord;

void main( void )
{
	gl_Position = projection * vec4( vertex.xy, -10, 1 );
	texcoord = coords;
}
