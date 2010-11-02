uniform mat4 projection;
uniform vec2 view;
attribute vec2 vertex;
varying vec2 coord;
varying vec2 pixel;

attribute vec2 scale;
attribute vec2 translate;
attribute float depth;

void main( void )
{
    vec4 point = vec4( vertex * scale + translate, depth, 1 );
    gl_Position = projection * ( point + vec4( view, 0, 0 ) );
    coord = vertex;
    pixel = 1. / scale;
}
