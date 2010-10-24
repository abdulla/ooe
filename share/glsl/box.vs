#extension GL_EXT_gpu_shader4 : enable

attribute vec2 vertex;
attribute vec2 scale;
attribute vec2 translate;
attribute float depth;
varying vec2 in_scale;
varying vec2 in_translate;
varying float in_depth;
varying float in_instance;

void main( void )
{
    gl_Position.xy = vertex;
    in_scale = scale;
    in_translate = translate;
    in_depth = depth;
    in_instance = float( gl_VertexID + 10 );
}
