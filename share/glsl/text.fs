/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform vsampler2D texture;
in vec2 coord;
in vec4 tint;
out vec4 colour;

void main( void )
{
    vec4 font = vtexelFetch( texture, coord );
    vec4 tone = vec4( 1. ); // vec4( background ) / 255.;
    colour = vec4( font.rgb * tint.rgb + ( 1. - font.rgb ) * tone.rgb, font.a * tint.a );
}
