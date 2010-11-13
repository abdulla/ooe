uniform mat4 projection;
uniform vec2 view;
uniform vec2 scale;
uniform vec2 translate;
uniform float depth;
attribute vec2 vertex;
varying vec2 coord;

void main( void )
{
    vec4 point = vec4( vertex * scale + translate, depth, 1 );
    gl_Position = projection * ( point + vec4( view, 0, 0 ) );
    coord = vertex;
}
