#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DArray sampler;
uniform int index;
varying vec2 texcoord;

void main( void )
{
    gl_FragColor = texture2DArray( sampler, vec3( texcoord, index ) );
}
