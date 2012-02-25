/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform mat4 projection;
uniform vec2 view;
uniform vec2 translate;
uniform int depth;
in vec2 vertex;
out vec2 coord;
out vec4 tint;

in vec2 vertex_scale;
in vec2 vertex_translate;
in vec2 coord_scale;
in vec2 coord_translate;
in vec4 colour;

void main( void )
{
    coord = vertex * coord_scale + coord_translate;
    tint = colour / 255.;
    vec2 point = vertex * vertex_scale + vertex_translate + translate;
    gl_Position = projection * vec4( point + view, depth, 1 );
}
