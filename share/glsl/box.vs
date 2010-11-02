uniform mat4 projection;
uniform vec2 view_translate;
attribute vec2 vertex;
attribute vec2 scale;
attribute vec2 translate;
attribute float depth;
varying vec2 coord;
varying vec2 pixel;

void main( void )
{
    vec4 transform = vec4( vertex * scale + translate + view_translate, depth, 1 );
    gl_Position = projection * transform;
    coord = vertex;
    pixel = 1. / scale;
}
