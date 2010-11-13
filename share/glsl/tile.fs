uniform vsampler2D texture;
varying vec2 coord;

void main( void )
{
    gl_FragColor = vtexture2D( texture, coord );
}
