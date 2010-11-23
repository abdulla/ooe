uniform mat4 projection;
uniform vec2 view;
uniform vec2 scale;
uniform vec2 translate;
uniform float depth;
uniform vec2 texcoord;
attribute vec2 vertex;
varying vec2 coord;

void main( void )
{
    vec2 point = vertex * scale + translate;
    gl_Position = projection * vec4( point + view, depth, 1 );
    coord = vertex * texcoord;
}
