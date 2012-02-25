/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform vsampler2D texture;
in vec2 coord;
out vec4 colour;

void main( void )
{
    colour = vtexture2D( texture, coord );
}
