uniform mat4 projection;
uniform mat4 model_view;
attribute vec2 vertex;
attribute vec2 coords;
varying vec2 texcoord;

void main( void )
{
    gl_Position = projection * model_view * vec4( vertex.xy, 0, 1 );
    texcoord = coords;
}
