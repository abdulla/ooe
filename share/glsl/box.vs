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
    vec2 point = vertex * scale + translate;
    gl_Position = projection * vec4( point + view, depth, 1 );
    coord = vertex;
    pixel = 1. / scale;
}
