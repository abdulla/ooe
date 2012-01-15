/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform mat4 projection;
uniform vec2 view;
uniform int depth;
attribute vec2 vertex;
varying vec4 tint;

attribute vec2 scale;
attribute vec2 translate;
attribute vec4 colour;

void main( void )
{
    vec2 point = vertex * scale + translate;
    gl_Position = projection * vec4( point + view, depth, 1 );
    tint = colour / 255.;
}
