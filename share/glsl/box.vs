uniform mat4 projection;
uniform mat4 model_view;
attribute vec2 vertex;
attribute vec2 coords;
attribute vec2 translate;
attribute vec2 scale;
attribute float z_index;
varying vec2 texcoord;

void main( void )
{
    vec2 transform = vertex * scale + translate;
    gl_Position = projection * model_view * vec4( transform, z_index, 1 );
    texcoord = coords;
}
