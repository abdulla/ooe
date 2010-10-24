varying vec2 coord;
varying float shadow;

void main( void )
{
    if ( shadow < .5 )
    {
        gl_FragColor = vec4( 1, 1, 1, 1 );
        return;
    }

    vec2 c = smoothstep( .2, 0., coord ) + smoothstep( .2, 0., vec2( 1, 1 ) - coord );
    float a = .75 - length( c );
    gl_FragColor = vec4( 0, 0, 0, a );
}
