uniform mat4 projection;
uniform vec4 vertex;

void main( void )
{
	gl_Position = projection * vertex;
}
