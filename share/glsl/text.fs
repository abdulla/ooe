uniform vsampler2D texture;
uniform ivec4 colour;
varying vec2 coord;

void main( void )
{
    vec4 fragment = vec4( colour ) / 255.;
    fragment.a *= vtexel2D( texture, coord ).a;
    gl_FragColor = fragment;
}
