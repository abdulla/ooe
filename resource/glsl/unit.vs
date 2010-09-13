#extension GL_EXT_gpu_shader4 : enable

uniform mat4 projection;
attribute vec2 vertex;
attribute vec2 coords;
varying vec2 texcoord;

void main( void )
{
	gl_Position = projection * vec4( vertex, 0, 1 );
	texcoord = coords;
}
