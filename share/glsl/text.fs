/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform vsampler2D texture;
in vec2 subtexel;
in vec2 coord;
in vec4 tint;
out vec4 colour;

vec4 rotate( vec4 texel, float value )
{
    switch ( int( value * 3 ) )
    {
    case 0:
        return texel;

    case 1:
        return texel.agba;

    case 2:
        return texel.aaba;
    }
}

void main( void )
{
    vec4 texel = vtexel2D( texture, coord );
    vec4 font = rotate( texel, subtexel.x );
    vec4 tone = vec4( 1. ); // vec4( background ) / 255.;
    colour = vec4( font.rgb * tint.rgb + ( 1. - font.rgb ) * tone.rgb, font.a * tint.a );
}
