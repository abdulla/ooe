varying vec2 texcoord;

void main( void )
{
    vec2 c = smoothstep( .8, 1., texcoord ) + smoothstep( .8, 1., vec2( 1, 1 ) - texcoord );
    float a = .8 - length( c );
    gl_FragColor = vec4( 1, 1, 1, a );
}
