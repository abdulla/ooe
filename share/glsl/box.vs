/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform mat4 projection;
uniform vec2 view;
uniform int depth;
in vec2 vertex;
out vec4 tint;

in vec2 scale;
in vec2 translate;
in vec4 colour;

void main( void )
{
    tint = colour / 255.;
    vec2 point = vertex * scale + translate;
    gl_Position = projection * vec4( point + view, depth, 1 );
}
