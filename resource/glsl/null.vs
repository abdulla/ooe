attribute vec2 vertex;
uniform mat4 projection;

void main( void )
{
	gl_Position = projection * vec4( vertex.x * 100., vertex.y * 100., -10, 1 );
}
