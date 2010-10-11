uniform vsampler2D vt;
uniform ivec3 colour;
varying vec2 texcoord;

void main( void )
{
    gl_FragColor = vec4( colour / 255, vtexture2D( vt, texcoord ).a );
}
