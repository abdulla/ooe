attribute vec2 vertex;
uniform mat4 projection;

void main( void )
{
	gl_Position = projection * vec4( vertex.xy, -10, 1 );
}
