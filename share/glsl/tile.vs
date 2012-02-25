/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform mat4 projection;
uniform vec2 view;
uniform vec2 scale;
uniform vec2 translate;
uniform int depth;
uniform vec2 texcoord;
in vec2 vertex;
out vec2 coord;

void main( void )
{
    coord = vertex * texcoord;
    vec2 point = vertex * scale + translate;
    gl_Position = projection * vec4( point + view, depth, 1 );
}
