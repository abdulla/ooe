uniform mat4 projection;
attribute vec2 vertex;

void main( void )
{
	gl_Position = projection * vec4( vertex.x, vertex.y, 0, 1 );
}
