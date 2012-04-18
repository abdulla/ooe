/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform vsampler2D texture;
in vec2 subtexel;
in vec2 coord_1;
in vec2 coord_2;
in vec4 tint;
out vec4 colour;

vec4 rotate( vec4 texel_1, vec4 texel_2, float value )
{
    vec3 rotated;

    switch ( int( value * 3. ) )
    {
    case 0:
        return texel_1;

    case 1:
        rotated = vec3( texel_2.b, texel_1.rg );

    case 2:
        rotated = vec3( texel_2.gb, texel_1.r );
    }

    float alpha = texel_1.a;
    return vec4( mix( texel_1.rgb, rotated, 1 - alpha ), alpha );
}

void main( void )
{
    vec4 texel_1 = vtexelFetch( texture, coord_1 );
    vec4 texel_2 = vtexelFetch( texture, coord_2 );
    vec4 font = rotate( texel_1, texel_2, subtexel.x );
    vec4 tone = vec4( 1. ); // vec4( background ) / 255.;
    colour = vec4( font.rgb * tint.rgb + ( 1. - font.rgb ) * tone.rgb, font.a * tint.a );
}
