uniform vsampler2D texture;
varying vec2 coord;
varying vec4 tint;

void main( void )
{
    vec4 font = vtexel2D( texture, coord );
    vec4 tone = vec4( 1. ); // vec4( background ) / 255.;
    gl_FragColor = vec4( font.rgb * tint.rgb + ( 1. - font.rgb ) * tone.rgb, font.a * tint.a );
}
