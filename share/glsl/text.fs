uniform vsampler2D texture;
uniform ivec4 colour;
varying vec2 coord;

void main( void )
{
    vec4 font = vtexel2D( texture, coord );
    vec4 tint = vec4( colour ) / 255.;
    gl_FragColor = vec4(
        font.r * tint.r + 1. - font.r,
        font.g * tint.g + 1. - font.g,
        font.b * tint.b + 1. - font.b,
        font.a * tint.a
    );
}
