uniform bool shadow;
varying vec2 coord;
varying vec2 pixel;

void main( void )
{
    vec4 colour;

    if ( !shadow )
        colour = vec4( 1, 1, 1, 1 );
    else
    {
        vec2 ten = pixel * 10.;
        float x = smoothstep( ten.x, 0., coord.x ) + smoothstep( ten.x, 0., 1. - coord.x );
        float y = smoothstep( ten.y, 0., coord.y ) + smoothstep( ten.y, 0., 1. - coord.y );
        float a = .25 - length( vec2( x, y ) );
        colour = vec4( 0, 0, 0, a );
    }

    gl_FragColor = colour;
}
