/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform vsampler2D texture;
varying vec2 coord;

void main( void )
{
    gl_FragColor = vtexture2D( texture, coord );
}
