uniform mat4 projection;
uniform vec2 view;
uniform vec2 translate;
uniform float depth;
attribute vec2 vertex;
varying vec2 coord;
varying vec4 tint;

attribute vec2 vertex_scale;
attribute vec2 vertex_translate;
attribute vec2 coord_scale;
attribute vec2 coord_translate;
attribute vec4 foreground;

void main( void )
{
    vec2 point = vertex * vertex_scale + vertex_translate + translate;
    gl_Position = projection * vec4( point + view, depth, 1 );
    coord = vertex * coord_scale + coord_translate;
    tint = foreground / 255.;
}
