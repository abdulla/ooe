uniform mat4 projection;
uniform vec3 view;
attribute vec2 vertex;
varying vec2 coord;

attribute vec2 vertex_scale;
attribute vec2 vertex_translate;
attribute vec2 coord_scale;
attribute vec2 coord_translate;

void main( void )
{
    vec4 point = vec4( vertex * vertex_scale + vertex_translate, 0, 1 );
    gl_Position = projection * ( point + vec4( view, 0 ) );
    coord = vertex * coord_scale + coord_translate;
}
