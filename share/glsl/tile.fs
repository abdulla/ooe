uniform vsampler2D texture;
uniform ivec3 colour;
varying vec2 coord;

void main( void )
{
    gl_FragColor = vec4( vec3( colour ) / 255., vtexture2D( texture, coord ).a );
}
