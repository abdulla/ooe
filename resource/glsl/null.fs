uniform vsampler2D vt;
varying vec2 texcoord;

void main( void )
{
    gl_FragColor = vtexture2D( vt, texcoord );
}
