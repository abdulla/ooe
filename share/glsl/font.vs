uniform mat4 projection;
uniform float depth;
attribute vec2 vertex;
varying vec2 texcoord;

attribute vec2 vertex_scale;
attribute vec2 vertex_translate;
attribute vec2 coords_scale;
attribute vec2 coords_translate;

void main( void )
{
    gl_Position = projection * vec4( vertex * vertex_scale + vertex_translate, depth, 1 );
    texcoord = vertex * coords_scale + coords_translate;
}
