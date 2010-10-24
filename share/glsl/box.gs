uniform mat4 projection;
uniform mat4 model_view;
varying in vec2 in_scale[];
varying in vec2 in_translate[];
varying in float in_depth[];
varying in float in_instance[];
varying out vec2 coord;
varying out float shadow;

void main( void )
{
    for ( int i = 0; i != gl_VerticesIn; ++i )
    {
        vec2 scale = in_scale[ i ] * 1.4;
        vec2 translate = in_translate[ i ] - in_scale[ i ] * .2;
        vec2 transform = gl_PositionIn[ i ].xy * scale + translate;
        vec4 vertex = vec4( transform, in_depth[ i ] - float( in_instance[ i ] ) / 100., 1 );

        gl_Position = projection * model_view * vertex;
        coord = gl_PositionIn[ i ].xy;
        shadow = 1.;

        EmitVertex();
    }

    EndPrimitive();

    for ( int i = 0; i != gl_VerticesIn; ++i )
    {
        vec2 transform = gl_PositionIn[ i ].xy * in_scale[ i ] + in_translate[ i ];
        vec4 vertex = vec4( transform, in_depth[ i ], 1 );

        gl_Position = projection * model_view * vertex;
        coord = gl_PositionIn[ i ].xy;
        shadow = 0.;

        EmitVertex();
    }

    EndPrimitive();
}
