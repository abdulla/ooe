uniform bool shadow;
varying vec2 coord;
varying vec2 pixel;

void main( void )
{
    vec4 fragment;

    if ( !shadow )
        fragment = vec4( 1, 1, 1, 1 );
    else
    {
        vec2 border = pixel * 10.;
        float x = smoothstep( border.x, -border.x, coord.x ) +
            smoothstep( border.x, -border.x, 1. - coord.x );
        float y = smoothstep( border.y, -border.y, coord.y ) +
            smoothstep( border.y, -border.y, 1. - coord.y );
        float a = .25 - length( vec2( x, y ) );
        fragment = vec4( 0, 0, 0, a );
    }

    gl_FragColor = fragment;
}
