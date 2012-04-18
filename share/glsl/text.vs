/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

uniform mat4 projection;
uniform vec2 view;
uniform vec2 translate;
uniform int depth;
in vec2 vertex;
out vec2 subtexel;
out vec2 coord_1;
out vec2 coord_2;
out vec4 tint;

in vec2 vertex_scale;
in vec2 vertex_translate;
in vec2 coord_scale;
in vec2 coord_translate;
in float texel_size;
in vec4 colour;

void main( void )
{
    subtexel = fract( vertex_translate );
    coord_1 = vertex * coord_scale + coord_translate;
    coord_2 = coord_1 - vec2( texel_size, 0 );
    tint = colour / 255.;
    vec2 point = vertex * vertex_scale + floor( vertex_translate ) + translate;
    gl_Position = projection * vec4( point + view, depth, 1 );
}
