uniform mat4 projection;
uniform mat4 model_view;
attribute vec2 vertex;
attribute vec2 scale;
attribute vec2 translate;
attribute float depth;
varying vec2 coord;
varying vec2 pixel;

void main( void )
{
    vec4 transform = vec4( vertex * scale + translate, depth, 1 );
    gl_Position = projection * model_view * transform;
    coord = vertex;
    pixel = 1. / scale;
}
