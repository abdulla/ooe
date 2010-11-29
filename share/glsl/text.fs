uniform vsampler2D texture;
varying vec2 coord;
varying vec4 tint;

void main( void )
{
    vec4 font = vtexel2D( texture, coord );
    gl_FragColor = vec4(
        font.r * tint.r + 1. - font.r,
        font.g * tint.g + 1. - font.g,
        font.b * tint.b + 1. - font.b,
        font.a * tint.a
    );
}
